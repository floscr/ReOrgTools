module DropDownMenu = {
  module Root = {
    [@bs.module "@radix-ui/react-dropdown-menu"] [@react.component]
    external make: (~children: React.element) => React.element = "Root";
  };

  module Trigger = {
    [@bs.module "@radix-ui/react-dropdown-menu"] [@react.component]
    external make: (~children: React.element) => React.element = "Trigger";
  };

  module Content = {
    [@bs.module "@radix-ui/react-dropdown-menu"] [@react.component]
    external make: (~children: React.element) => React.element = "Content";
  };

  module Item = {
    [@bs.module "@radix-ui/react-dropdown-menu"] [@react.component]
    external make: (~children: React.element) => React.element = "Item";
  };
};
