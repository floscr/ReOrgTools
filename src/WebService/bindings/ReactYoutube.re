[@bs.module "react-youtube"] [@react.component]
external make:
  (~videoId: string, ~className: string=?, ~onReady: Js.t('a) => unit=?) =>
  React.element =
  "default";
