module Label = {
  module Root = {
    [@bs.module "@radix-ui/react-label"] [@react.component]
    external make:
      (~children: React.element, ~htmlFor: string=?, ~className: string=?) =>
      React.element =
      "Root";
  };
};
