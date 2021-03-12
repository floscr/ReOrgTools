open ReOrga;

type pageLoadedState = {
  text: string,
  ast: ReOrga.orgAst,
};

type pageState =
  | FetchedPage(pageLoadedState)
  | FetchingPage
  | EmptyPage
  | NotFoundPage;

type globalState = {page: pageState};

let initialGlobalState = {page: EmptyPage};

type action =
  | FetchPagesSuccess(PageTypes.PageType.t)
  | FetchPagesFailure(ReludeFetch.Error.t(string))
  | NoOp;

let reducer =
    (state: globalState, action: action)
    : ReludeReact.Reducer.update(action, globalState) =>
  switch (action) {
  | FetchPagesSuccess({text}) =>
    Update({
      ...state,
      page:
        FetchedPage({
          text,
          ast: Org.parseOrga(text, {todo: Some([|"TODO"|])}),
        }),
    })
  | NoOp => NoUpdate
  | _ => NoUpdate
  };
