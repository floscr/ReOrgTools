module Implementation = {
  type state = {dialogsState: ReductiveStore__Dialogs.state};

  type action =
    | DialogsAction(ReductiveStore__Dialogs.action);

  let reducer = (state, action) =>
    switch (action) {
    | DialogsAction(a) => {
        ...state,
        dialogsState: ReductiveStore__Dialogs.reducer(state.dialogsState, a),
      }
    };
  let store =
    Reductive.Store.create(
      ~reducer,
      ~preloadedState={dialogsState: ReductiveStore__Dialogs.initial},
      (),
    );

  module Selector = {
    module DialogsStore = {
      let dialogs = state => state.dialogsState.dialogs;
    };
  };
};
include Implementation;

module Wrapper = {
  include ReductiveContext.Make(Implementation);
};
