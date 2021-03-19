module Implementation = {
  type state = {
    dialogsState: ReductiveStore__Dialogs.state,
    workspacesState: ReductiveStore__Workspaces.Store.state,
  };

  type action =
    | DialogsAction(ReductiveStore__Dialogs.action)
    | WorkspaceAction(ReductiveStore__Workspaces.Store.action);

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
    };

  let store =
    Reductive.Store.create(
      ~reducer,
      ~preloadedState={
        dialogsState: ReductiveStore__Dialogs.initial,
        workspacesState: ReductiveStore__Workspaces.Store.initialState,
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
  };
};
include Implementation;

module Wrapper = {
  include ReductiveContext.Make(Implementation);
};
