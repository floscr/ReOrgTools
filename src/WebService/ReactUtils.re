open Relude.Globals;

module JsonParser = {
  // Regex taken from https://github.com/nokazn/strip-json-trailing-commas/blob/main/src/index.ts
  // Parses json, if the first parse fails it removes all trailing commas and parses again.
  // Adds convinience factor for editing json, should not be used on large strings.
  let parseIgnoreTrailing = (x: string): option(Js.Json.t) =>
    x
    |> Json.parse
    |> Option.orElseLazy(~fallback=_ =>
         x
         |> String.replaceRegex(
              ~search=[%re
                "/(?<=(true|false|null|[\\\"\d}\]])\s*)\s*,(?=\s*[}\]])/g"
              ],
              ~replaceWith="",
            )
         |> Json.parse
       );
};

[@bs.send] external blur: Js.t({..}) => unit = "blur";

let blurOnClick = e => e |> ReactEvent.Mouse.currentTarget |> blur;

let identity: 'a => 'a = x => x;

let tap = (fn: 'a => unit, x: 'a) => {
  fn(x);
  x;
};

let omit = (f, _) => f();

module ClassName = {
  include Relude.Identity;
  let id = x => x;

  let append = className => map(a => a ++ " " ++ className);

  let condAppend = (cond, className) => cond ? append(className) : id;
};

let s = React.string;

[@bs.send] external performanceNow: Webapi__Performance.t => float = "now";

/* Logs the time (in ms) it takes to run a function, using the DOM Performance API. */
[@live]
let measure = f => {
  let performance = Webapi.Dom.window |> Webapi.Dom.Window.performance;
  let t0 = performanceNow(performance);
  let v = f();
  let t1 = performanceNow(performance);
  Js.log("Measurement: " ++ (t1 -. t0 |> Js.Float.toString) ++ "ms.");
  v;
};

module Icon = {
  [@react.component]
  let make = (~id) => {
    <span className="material-icons"> {id |> s} </span>;
  };
};

module Styles = {
  open Css;
  open FixedTheme;

  let iconButton =
    style([
      userSelect(`none),
      lineHeight(`abs(1.)),
      position(relative),
      selector(
        ":hover::after, :focus::after, :focus::before",
        [
          unsafe("content", ""),
          position(absolute),
          top(Spacing.negativeSmall),
          left(Spacing.negativeSmall),
          right(Spacing.negativeSmall),
          bottom(Spacing.negativeSmall),
          borderRadius(BorderRadius.small),
          border(px(1), `solid, var(ThemeKeys.baseGray1)),
        ],
      ),
      selector(":focus::before", [opacity(0.15), borderWidth(px(3))]),
      selector(
        ":focus::after, :focus::before",
        [borderColor(var(ThemeKeys.focus))],
      ),
      selector(
        ":focus",
        [outlineStyle(none), color(var(ThemeKeys.focus))],
      ),
    ]);
};

module IconButton = {
  let buttonClassName = Styles.iconButton;

  [@react.component]
  let make = (~id, ~style=?, ~onClick=?) => {
    let className =
      switch (style) {
      | Some(x) => buttonClassName ++ " " ++ x
      | _ => buttonClassName
      };
    <button className onMouseUp=blurOnClick ?onClick> <Icon id /> </button>;
  };
};

let extractDomElementFromRef = (reactRef: React.ref(Js.Nullable.t('a))) =>
  reactRef.current->Js.Nullable.toOption;

let scrollIntoViewIfNeeded =
    (inputRef: React.ref(Js.Nullable.t(Dom.element))) => {
  switch (extractDomElementFromRef(inputRef)) {
  | Some(el) =>
    Js.Global.setTimeout(
      () => ScrollIntoViewIfNeeded.scrollIntoViewIfNeeded(el),
      10,
    )
    ->ignore

  | None => ()
  };
};

let eventPrevent = (fn, event) => {
  event->ReactEvent.Synthetic.preventDefault;
  event->fn;
};

module Form = {
  module Style = {
    open Css;
    open FixedTheme;

    let form =
      style([
        selector(
          "input, textarea",
          [
            padding(Spacing.medium),
            color(var(ThemeKeys.baseGray12)),
            borderRadius(BorderRadius.small),
            fontSize(rem(0.9)),
          ],
        ),
        selector(
          "input[type='submit']",
          [
            borderStyle(none),
            fontWeight(bolder),
            backgroundColor(var(ThemeKeys.baseGray3)),
            padding2(~v=Spacing.medium, ~h=Spacing.xlarge),
            not_(
              ":disabled",
              [
                hover([
                  backgroundColor(var(ThemeKeys.accentMain)),
                  color(white),
                  cursor(pointer),
                ]),
              ],
            ),
            disabled([opacity(0.3)]),
          ],
        ),
        selector(
          "textarea, input[type='text']",
          [
            backgroundColor(var(ThemeKeys.bgColor)),
            fontFamily(`custom("Ubuntu Mono")),
            lineHeight(`abs(1.42)),
            border(px(1), `solid, var(ThemeKeys.baseGray7)),
            focus([
              outlineWidth(zero),
              borderColor(var(ThemeKeys.accentMain)),
            ]),
          ],
        ),
      ]);
  };

  module Wrapper = {
    [@react.component]
    let make = (~onSubmit=?, ~children) => {
      <form ?onSubmit className=Style.form> children </form>;
    };
  };

  module SubmitButton = {
    [@react.component]
    let make = (~disabled: option(bool)=?) => {
      <input type_="submit" value="Submit" ?disabled />;
    };
  };
};
