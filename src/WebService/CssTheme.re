open Relude.Globals;

type cssVar = (string, string);

module type Colors = {
  let key: string;

  let bgColor: cssVar;
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
    | "light" => Light
    | "dark" => Dark
    | _ as x => failwith({j|Unknown theme: $(x)|j});

  let getModule = (theme): (module Colors) =>
    switch (theme) {
    | Light => (module LightTheme)
    | Dark => (module DarkTheme)
    };

  let getStorage = () =>
    Dom.Storage.localStorage
    |> Dom.Storage.getItem(storageKey)
    |> Option.map(decodeThemeKey)
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
       });
  };

  let currentTheme = () => {
    getRootHtmlElement()
    |> Option.map(getCssVar("--theme"))
    |> Option.filter(String.isEmpty)
    |> Option.map(Theme.decodeThemeKey)
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
          theme |> Dom.setTheme;
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
