module ScrollArea = {
  module Root = {
    [@bs.module "@radix-ui/react-scroll-area"] [@react.component]
    external make:
      (~children: React.element, ~className: string=?) => React.element =
      "Root";
  };

  module Viewport = {
    [@bs.module "@radix-ui/react-scroll-area"] [@react.component]
    external make:
      (~children: React.element, ~className: string=?) => React.element =
      "Viewport";
  };

  module Scrollbar = {
    [@bs.module "@radix-ui/react-scroll-area"] [@react.component]
    external make:
      (
        ~children: React.element,
        ~className: string=?,
        ~orientation_: string=?
      ) =>
      React.element =
      "Scrollbar";
  };

  module Thumb = {
    [@bs.module "@radix-ui/react-scroll-area"] [@react.component]
    external make: (~className: string=?) => React.element = "Thumb";
  };

  module Corner = {
    [@bs.module "@radix-ui/react-scroll-area"] [@react.component]
    external make: (~className: string=?) => React.element = "Corner";
  };
};
