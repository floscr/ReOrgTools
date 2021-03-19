open Relude.Globals;

type cssVar = (string, string);

module type Colors = {
  let key: string;

  let bgColor: cssVar;
  let textColor: cssVar;

  let grey00: cssVar;
  let grey10: cssVar;
  let grey20: cssVar;
  let grey30: cssVar;

  let blue: cssVar;
};

module Theme = {
  type t =
    | Light
    | Dark;

  let default = Light;
  let storageKey = "theme";

  let encodeThemeKey =
    fun
    | Light => "light"
    | Dark => "dark";

  let decodeThemeKey =
    fun
    | "light" => Some(Light)
    | "dark" => Some(Dark)
    | _ => None;

  let getModule = (theme): (module Colors) =>
    switch (theme) {
    | Light => (module LightTheme)
    | Dark => (module DarkTheme)
    };

  let getStorage = () =>
    Dom.Storage.localStorage
    |> Dom.Storage.getItem(storageKey)
    |> Option.flatMap(decodeThemeKey)
    |> Option.getOrElse(default);

  let setStorage = x =>
    Dom.Storage.localStorage
    |> Dom.Storage.setItem(storageKey, x |> encodeThemeKey);
};

module Dom = {
  let getRootHtmlElement = () => {
    Webapi__Dom.(
      window
      |> Window.document
      |> Document.documentElement
      |> Element.asHtmlElement
    );
  };

  let getCssVar = (key, el) => {
    Webapi__Dom.(
      el |> HtmlElement.style |> CssStyleDeclaration.getPropertyValue(key)
    );
  };

  let setCssVar = ((key, value), el) => {
    Webapi__Dom.(
      el
      |> HtmlElement.style
      |> CssStyleDeclaration.setProperty(key, value, "")
    );
  };

  let setTheme = theme => {
    let (module ThemeModule) = theme |> Theme.getModule;

    getRootHtmlElement()
    |> Option.tap(el => {
         el |> setCssVar(("--theme", ThemeModule.key));
         el |> setCssVar(ThemeModule.bgColor);
         el |> setCssVar(ThemeModule.textColor);
         el |> setCssVar(ThemeModule.grey00);
         el |> setCssVar(ThemeModule.grey10);
         el |> setCssVar(ThemeModule.grey20);
         el |> setCssVar(ThemeModule.grey30);
         el |> setCssVar(ThemeModule.blue);
         ();
       });
  };

  let setDefaultTheme = () => Theme.getStorage() |> setTheme |> ignore;

  let currentTheme = () => {
    getRootHtmlElement()
    |> Option.map(getCssVar("--theme"))
    |> Option.filter(String.isEmpty)
    |> Option.flatMap(Theme.decodeThemeKey)
    |> Option.getOrElse(Theme.default);
  };
};

type ctx = {
  current: Theme.t,
  colors: (module Colors),
  set: Theme.t => unit,
};

module Context = {
  let makeProps = (~value, ~children, ()) => {
    "value": value,
    "children": children,
  };
  let themeContext =
    React.createContext({
      current: Theme.default,
      colors: Theme.default |> Theme.getModule,
      set: ignore,
    });
  include React.Context; // Adds the makeProps external
  let make = React.Context.provider(themeContext);
};

module Provider = {
  type action =
    | Set(Theme.t);

  let reducer = (_state, action) =>
    switch (action) {
    | Set(theme) => theme
    };

  [@react.component]
  let make = (~children) => {
    let mounted = React.useRef(false);
    let theme = React.useMemo0(Dom.currentTheme);
    let (theme, dispatch) = reducer->React.useReducer(theme);

    React.useEffect1(
      () => {
        if (mounted.current) {
          theme |> Theme.setStorage;
          theme |> Dom.setTheme |> ignore;
          ();
        } else {
          mounted.current = true;
        };
        None;
      },
      [|theme|],
    );

    let value =
      React.useMemo1(
        () =>
          {
            current: theme,
            colors: theme |> Theme.getModule,
            set: theme => Set(theme) |> dispatch,
          },
        [|theme|],
      );

    <Context value> children </Context>;
  };
};
