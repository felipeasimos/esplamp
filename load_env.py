from dotenv import dotenv_values


vars = dotenv_values(".env")
var_flags = [f"-D {k}=\"\"{v}\"\"" for k, v in vars.items()]
print(var_flags)

# make .env variables avaiable to the C preprocessor
Import("env")
env.Append(CCFLAGS=var_flags)
