module Implementation = {
  type state = {
    dialogsState: ReductiveStore__Dialogs.state,
    workspacesState: ReductiveStore__Workspaces.Store.state,
    orgDocumentsState: ReductiveStore__OrgDocuments.Store.state,
  };

  type action =
    | DialogsAction(ReductiveStore__Dialogs.action)
    | WorkspaceAction(ReductiveStore__Workspaces.Store.action)
    | OrgDocumentsAction(ReductiveStore__OrgDocuments.Store.action);

  let reducer = (state, action) =>
    switch (action) {
    | DialogsAction(a) => {
        ...state,
        dialogsState: ReductiveStore__Dialogs.reducer(state.dialogsState, a),
      }
    | WorkspaceAction(a) => {
        ...state,
        workspacesState:
          ReductiveStore__Workspaces.Store.reducer(state.workspacesState, a),
      }
    | OrgDocumentsAction(a) => {
        ...state,
        orgDocumentsState:
          ReductiveStore__OrgDocuments.Store.reducer(
            state.orgDocumentsState,
            a,
          ),
      }
    };

  let store =
    Reductive.Store.create(
      ~reducer,
      ~preloadedState={
        dialogsState: ReductiveStore__Dialogs.initial,
        workspacesState: ReductiveStore__Workspaces.Store.initialState,
        orgDocumentsState: ReductiveStore__OrgDocuments.Store.initialState,
      },
      (),
    );

  module Selector = {
    module DialogsStore = {
      let dialogs = state => state.dialogsState.dialogs;
    };
    module WorkspacesStore = {
      let workspaces = state => state.workspacesState.workspaces;
    };
    module OrgDocumentsStore = {
      let files = state => state.orgDocumentsState.filesCache;
    };
  };
};

include Implementation;

module Wrapper = {
  include ReductiveContext.Make(Implementation);
};
