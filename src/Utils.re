let log = x => {
  Js.log(x);
  x;
};

let mapOption = (f, x) =>
  switch (x) {
  | Some(x) => f(x)
  | None => x
  };

let nullableOrEmptyArray = x =>
  switch (Js.Nullable.toOption(x)) {
  | Some(xs) => xs
  | _ => [||]
  };

let nullableOrEmptyStr = x =>
  switch (Js.Nullable.toOption(x)) {
  | Some((x: string)) => x
  | _ => ""
  };

let nullableOrZero = x =>
  switch (Js.Nullable.toOption(x)) {
  | Some((x: int)) => x
  | _ => 0
  };

let nullableOrBool = (x, fallback: bool) =>
  switch (Js.Nullable.toOption(x)) {
  | Some((x: bool)) => x
  | _ => fallback
  };

let nullableOrEmptyDict = x =>
  switch (Js.Nullable.toOption(x)) {
  | Some((x: Js.Dict.t(string))) => x
  | _ => Js.Dict.empty()
  };
