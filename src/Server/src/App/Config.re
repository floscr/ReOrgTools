Dotenv.config();

let env = Node.Process.process##env;

let orgDir = Js.Dict.unsafeGet(env, "ORG_DIR");
