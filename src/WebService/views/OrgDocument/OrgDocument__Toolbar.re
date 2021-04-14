open Relude.Globals;
open ReOrga;
open ReactUtils;
open OrgDocument__Utils;

module Styles = {
  open Css;
  open FixedTheme;
  let root =
    style([
      position(sticky),
      top(zero),
      padding(FixedTheme.Spacing.large),
      borderBottom(px(1), solid, var(ThemeKeys.grey10)),
      flexShrink(1.),
      flexGrow(1.),
      display(`flex),
      backgroundColor(var(ThemeKeys.bgColor)),
      justifyContent(spaceBetween),
      alignItems(center),
    ]);

  let selectWrapper = style([flex(`num(1.)), maxWidth(px(250))]);
};

[@react.component]
let make =
    (~ast: ReOrga.orgAst, ~queryParams: Types__URLSearchParams.t, ~layoutType) => {
  open Types__URLSearchParams.Layouts;

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
    <div>
      {ast.properties->Js.Dict.get("title")
       |> Option.getOrElse(String.empty)
       |> s}
    </div>
    <div className=Styles.selectWrapper>
      <ReactSelect
        options=reactSelectOptions
        value={layoutType |> toReactSelect}
        isSearchable=false
        onChange
      />
    </div>
  </header>;
};
