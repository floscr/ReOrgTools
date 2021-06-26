module RadioGroup = {
  module Root = {
    [@bs.module "@radix-ui/react-radio-group"] [@react.component]
    external make: (~children: React.element) => React.element = "Root";
  };

  module Indicator = {
    [@bs.module "@radix-ui/react-radio-group"] [@react.component]
    external make: (~className: string=?) => React.element = "Indicator";
  };

  module Item = {
    [@bs.module "@radix-ui/react-radio-group"] [@react.component]
    external make:
      (
        ~children: React.element,
        ~value: string=?,
        ~name: string=?,
        ~className: string=?
      ) =>
      React.element =
      "Item";
  };
};
