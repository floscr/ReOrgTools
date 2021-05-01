[@react.component]
let make = () => {
  open State;
  open State__User;
  let user = Store.useSelector(Selector.User.user);
  switch (user) {
  | LoggedInUser(_) => <Router__LoggedIn />
  | _ => <Router__Unauthorized />
  };
};
