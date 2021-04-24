open Relude.Globals;
open ReOrga;
open ReactUtils;
open OrgDocument__Utils;
open State;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style([
      position(sticky),
      top(zero),
      padding(FixedTheme.Spacing.medium),
      borderBottom(px(1), solid, var(ThemeKeys.grey10)),
      flexShrink(1.),
      display(`flex),
      backgroundColor(var(ThemeKeys.bgColor)),
      justifyContent(spaceBetween),
      alignItems(center),
    ]);

  let menuIcon = style([marginRight(Spacing.medium)]);

  let leftRoot = style([display(flexBox), alignItems(center)]);

  let selectWrapper = style([flex(`num(1.)), maxWidth(px(250))]);
};

[@react.component]
let make = (~ast: ReOrga.orgAst, ~layoutType) => {
  open Types__URLSearchParams.Layouts;
  let isSidebarOpen = Store.useSelector(Selector.Settings.isSidebarOpen);
  let dispatch = State.Store.useDispatch();

  let onChange = (x: Js.Nullable.t(ReactSelect.t)) =>
    x
    |> Js.Nullable.toOption
    |> Option.tap(({value}: ReactSelect.t) =>
         ReludeURL.(
           Webapi.Dom.(
             window
             |> Window.location
             |> Location.href
             |> (x => URI.parser |> ReludeParse.Parser.runParser(x))
             |> Result.map(
                  URI.setQueryParam(
                    QueryParam.make1(
                      QueryKey.make("layoutType"),
                      QueryValue.make(value),
                    ),
                  ),
                )
             |> Result.map(URI.show)
             |> Result.tap(ReasonReactRouter.replace)
           )
         )
         |> ignore
       )
    |> ignore;

  <header className=Styles.root>
    <div className=Styles.leftRoot>
      <IconButton
        id={isSidebarOpen ? "menu_open" : "menu"}
        style=Styles.menuIcon
        onClick={_ =>
          SettingsAction(State__Settings.ToggleSidebar) |> dispatch
        }
      />
      {ast.properties->Js.Dict.get("title")
       |> Option.getOrElse(String.empty)
       |> s}
    </div>
    <div
      className=Styles.selectWrapper
      /* React Select Breaks Styling, as it uses emotion v11  */
      /* Look for https://github.com/reasonml-labs/bs-css/pull/229 */
      /* <ReactSelect */
      /*   options=reactSelectOptions */
      /*   value={layoutType |> toReactSelect} */
      /*   isSearchable=false */
      /*   onChange */
      /* /> */
    />
  </header>;
};
