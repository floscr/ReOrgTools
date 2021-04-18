module Routes = {
  let fileUrl = (~workspaceIndex: int, ~id: string) => {j|/file/$workspaceIndex/$id|j};

  let openFile = (~workspaceIndex: int, ~id: string) =>
    ReasonReactRouter.replace(fileUrl(~workspaceIndex, ~id));

};
