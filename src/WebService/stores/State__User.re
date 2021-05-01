open Relude.Globals;
open ReactUtils;

module User = {
  type t = {
    jwt: string,
    username: string,
  };
};

type user =
  | LoggedInUser(User.t)
  | Unauthorized;

type state = {user};

let initialState = {user: Unauthorized};

type action =
  | Login(User.t)
  | Logout;

let reducer = (state, action) => {
  switch (action) {
  | Login(user) => {user: LoggedInUser(user)}
  | Logout => {user: Unauthorized}
  };
};
