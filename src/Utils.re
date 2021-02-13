let log = x => {
  Js.log(x);
  x;
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
