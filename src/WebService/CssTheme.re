open Relude.Globals;

type t =
  | Light
  | Dark;

let default = Light;

module type Colors = {
  let key: string;

  let bgColor: string;
};

let toString =
  fun
  | Light => "light"
  | Dark => "dark";

let fromString =
  fun
  | "light" => Light
  | "dark" => Dark
  | _ as x => failwith({j|Unknown theme: $(x)|j});

let getModule = (theme): (module Colors) =>
  switch (theme) {
  | Light => (module LightTheme)
  | Dark => (module DarkTheme)
  };

module Storage = {
  let key = "theme";

  let getTheme = () =>
    Web.Storage.localStorage
    ->Web.Storage.getItem(key)
    ->Option.map(fromString);

  let setTheme = value =>
    Web.Storage.localStorage->Web.Storage.setItem(key, value->toString);
};

module Dom = {
  let darkMq = "(prefers-color-scheme: dark)";
  let lightMq = "(prefers-color-scheme: light)";

  let getRoot = () => {
    Web.Dom.(
      window
      ->Window.document
      ->Document.documentElement
      ->Element.unsafeAsHtmlElement
    );
  };

  let getVar = (root, key) => {
    Web.Dom.(
      root->HtmlElement.style->CssStyleDeclaration.getPropertyValue(key, _)
    );
  };

  let setVar = (root, key, value) => {
    Web.Dom.(
      root
      ->HtmlElement.style
      ->CssStyleDeclaration.setProperty(key, value, "", _)
    );
  };

  let setTheme = theme => {
    let (module Theme) = theme->getModule;

    let root = getRoot();

    root->setVar("--theme", Theme.key);
    root->setVar("--bg-color", Theme.bgColor);
  };

  let currentTheme = () => {
    let root = getRoot();

    switch (root->getVar("--theme")) {
    | "" => default
    | _ as x => x->fromString
    };
  };
};

let setDefault = () => {
  switch (Storage.getTheme()) {
  | Some(theme) => theme->Dom.setTheme
  | None =>
    let darkMq = Web.Dom.(window->Window.matchMedia(Dom.darkMq, _));
    if (darkMq->MediaQueryList.matches) {
      Dark->Dom.setTheme;
    } else {
      Light->Dom.setTheme;
    };
  };
};

type ctx = {
  current: t,
  colors: (module Colors),
  set: t => unit,
};

module Context = {
  include ReactContext.Make({
    type context = ctx;
    let defaultValue = {
      current: default,
      colors: default->getModule,
      set: ignore,
    };
  });
};

module Provider = {
  type action =
    | Set(t);

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
          theme->Storage.setTheme;
          theme->Dom.setTheme;
        } else {
          mounted.current = true;
        };
        None;
      },
      [|theme|],
    );

    React.useEffect1(
      () =>
        Subscription.onKeyDown(event =>
          Web.Dom.(
            switch (event->KeyboardEvent.code) {
            | "KeyT" when event->KeyboardEvent.altKey =>
              switch (theme) {
              | Light => Set(Dark)->dispatch
              | Dark => Set(Light)->dispatch
              }
            | _ => ()
            }
          )
        ),
      [|theme|],
    );

    let value =
      React.useMemo1(
        () =>
          {
            current: theme,
            colors: theme->getModule,
            set: theme => Set(theme)->dispatch,
          },
        [|theme|],
      );

    <Context.Provider value> children </Context.Provider>;
  };
};
