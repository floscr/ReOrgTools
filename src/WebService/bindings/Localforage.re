/**
 * For documentation go to :
 * https://localforage.github.io/localForage/
 */
type localforage;

[@bs.deriving jsConverter]
type driver = [
  | [@bs.as "localforage.INDEXEDDB"] `indexeddb
  | [@bs.as "localforage.WEBSQL"] `websql
  | [@bs.as "localforage.LOCALSTORAGE"] `localstorage
];

[@bs.val] [@bs.module "localforage"]
external getItem: string => Js.Promise.t(Js.nullable('a)) = "getItem";

[@bs.val] [@bs.module "localforage"]
external setItem: (string, 'a) => Js.Promise.t(unit) = "setItem";

[@bs.val] [@bs.module "localforage"]
external removeItem: string => Js.Promise.t(unit) = "removeItem";

[@bs.val] [@bs.module "localforage"]
external clear: unit => Js.Promise.t(unit) = "clear";

[@bs.val] [@bs.module "localforage"]
external length: unit => Js.Promise.t(int) = "length";

[@bs.val] [@bs.module "localforage"]
external key: int => Js.Promise.t(string) = "key";

[@bs.val] [@bs.module "localforage"]
external keys: unit => Js.Promise.t(array(string)) = "keys";

[@bs.val] [@bs.module "localforage"]
external iterate:
  ((~value: 'a, ~key: string, ~iterationNumber: int) => unit) =>
  Js.Promise.t(unit) =
  "iterate";

[@bs.val] [@bs.module "localforage"]
external setDriver: driver => unit = "setDriver";

[@bs.val] [@bs.module "localforage"]
external setDrivers: array(driver) => unit = "setDriver";

module Localforage_IO = {
  open Relude.Globals;

  type errT =
    | Errored(Js.Promise.error)
    | LocalForageError(string);

  let set = (key, value) =>
    setItem(key, value)
    |> Relude.Js.Promise.toIO
    |> IO.mapError(_ => "Get PromiseError");

  let keys = () =>
    keys() |> Relude.Js.Promise.toIO |> IO.mapError(_ => "Keys PromiseError");

  let get = key =>
    getItem(key)
    |> Relude.Js.Promise.toIO
    // Convert error type to string
    |> IO.mapError(_ => "")
    |> IO.condError(
         Js.Nullable.toOption >> Option.isSome,
         {j|Key "$key" not found in db|j},
       );
};
