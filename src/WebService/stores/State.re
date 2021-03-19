module Implementation = {
  type state = {
    dialogsState: State__Dialogs.state,
    workspacesState: State__Workspaces.Store.state,
    orgDocumentsState: State__OrgDocuments.Store.state,
  };

  type action =
    | DialogsAction(State__Dialogs.action)
    | WorkspaceAction(State__Workspaces.Store.action)
    | OrgDocumentsAction(State__OrgDocuments.Store.action);

  let reducer = (state, action) =>
    switch (action) {
    | DialogsAction(a) => {
        ...state,
        dialogsState: State__Dialogs.reducer(state.dialogsState, a),
      }
    | WorkspaceAction(a) => {
        ...state,
        workspacesState:
          State__Workspaces.Store.reducer(state.workspacesState, a),
      }
    | OrgDocumentsAction(a) => {
        ...state,
        orgDocumentsState:
          State__OrgDocuments.Store.reducer(state.orgDocumentsState, a),
      }
    };

  let store =
    Reductive.Store.create(
      ~reducer,
      ~preloadedState={
        dialogsState: State__Dialogs.initial,
        workspacesState: State__Workspaces.Store.initialState,
        orgDocumentsState: State__OrgDocuments.Store.initialState,
      },
      (),
    );

  module Selector = {
    module Dialogs = {
      let dialogs = state => state.dialogsState.dialogs;
    };
    module Workspaces = {
      let workspaces = state => state.workspacesState.workspaces;
    };
    module OrgDocuments = {
      let files = state => state.orgDocumentsState.filesCache;
    };
  };
};

include Implementation;

module Store = {
  include ReductiveContext.Make(Implementation);
};

module Stores = {
  module Workspaces = State__Workspaces;
  module OrgDocument = State__OrgDocuments;
  module Dialogs = State__Dialogs;
};
