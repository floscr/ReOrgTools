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
      borderBottom(px(1), solid, var(ThemeKeys.baseGray1)),
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
  open Radix__DropDownMenu;

  let isSidebarOpen = Store.useSelector(Selector.Settings.isSidebarOpen);
  let dispatch = State.Store.useDispatch();

  let onChange = x => {
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
                 QueryValue.make(
                   x |> Types__URLSearchParams.Layouts.toString,
                 ),
               ),
             ),
           )
        |> Result.map(URI.show)
        |> Result.tap(ReasonReactRouter.replace)
      )
    )
    |> ignore;
  };

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
    <div className=Styles.selectWrapper>
      <DropDownMenu.Root>
        <DropDownMenu.Trigger> {"Trigger" |> s} </DropDownMenu.Trigger>
        <DropDownMenu.Content>
          {Types__URLSearchParams.(
             Layouts.options
             |> Array.map(x =>
                  <React.Fragment key=x>
                    <DropDownMenu.Item onSelect={_ => onChange(x)}>
                      {x |> Layouts.toString |> s}
                    </DropDownMenu.Item>
                  </React.Fragment>
                )
           )
           |> React.array}
        </DropDownMenu.Content>
      </DropDownMenu.Root>
    </div>
  </header>;
};
