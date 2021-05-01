module Implementation = {
  type state = {
    dialogsState: State__Dialogs.state,
    orgDocumentsState: State__OrgDocuments.Store.state,
    settingsState: State__Settings.state,
    workspacesState: State__Workspaces.Store.state,
    userState: State__User.state,
  };

  type action =
    | DialogsAction(State__Dialogs.action)
    | OrgDocumentsAction(State__OrgDocuments.Store.action)
    | SettingsAction(State__Settings.action)
    | WorkspaceAction(State__Workspaces.Store.action)
    | UserAction(State__User.action);

  let reducer = (state, action) =>
    switch (action) {
    | DialogsAction(a) => {
        ...state,
        dialogsState: State__Dialogs.reducer(state.dialogsState, a),
      }
    | OrgDocumentsAction(a) => {
        ...state,
        orgDocumentsState:
          State__OrgDocuments.Store.reducer(state.orgDocumentsState, a),
      }
    | SettingsAction(a) => {
        ...state,
        settingsState: State__Settings.reducer(state.settingsState, a),
      }
    | WorkspaceAction(a) => {
        ...state,
        workspacesState:
          State__Workspaces.Store.reducer(state.workspacesState, a),
      }
    | UserAction(a) => {
        ...state,
        userState: State__User.reducer(state.userState, a),
      }
    };

  let store =
    Reductive.Store.create(
      ~reducer,
      ~preloadedState={
        dialogsState: State__Dialogs.initial,
        orgDocumentsState: State__OrgDocuments.Store.initialState,
        settingsState: State__Settings.initialState,
        workspacesState: State__Workspaces.Store.initialState,
        userState: State__User.initialState,
      },
      (),
    );

  module Selector = {
    module Dialogs = {
      let dialogs = state => state.dialogsState.dialogs;
    };
    module OrgDocuments = {
      let files = state => state.orgDocumentsState.filesCache;
    };
    module Settings = {
      let settings = state => state.settingsState;
      let bookmarks = state => state.settingsState.bookmarks;
      let isSidebarOpen = state => state.settingsState.isSidebarOpen;
    };
    module Workspaces = {
      let workspaces = state => state.workspacesState.workspaces;
    };
    module User = {
      let user = state => state.userState.user;
    };
  };
};

include Implementation;

module Store = {
  include ReductiveContext.Make(Implementation);
};

module Stores = {
  module Dialogs = State__Dialogs;
  module OrgDocument = State__OrgDocuments;
  module Settings = State__Settings;
  module Workspaces = State__Workspaces;
  module User = State__User;
};
