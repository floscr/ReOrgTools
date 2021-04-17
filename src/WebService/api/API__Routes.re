module Routes = {
  let openFile = (~workspaceIndex: int, ~id: string) =>
    ReasonReactRouter.replace(
      State__OrgDocuments.File.encodeUrlId(~workspaceIndex, ~id),
    );
};
