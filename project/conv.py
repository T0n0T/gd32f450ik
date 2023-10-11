import re
import sys

# 读取文件内容
file_path = sys.argv[1] + "\\compile_commands.json"
with open(file_path, "r") as file:
    file_contents = file.read()

# 替换斜杠
file_contents = file_contents.replace("\\\\", "/")

# 替换盘符
file_contents = re.sub(
    r"([a-zA-Z]):", lambda x: x.group(1).upper() + ":", file_contents
)

# 将处理后的内容写回文件
with open(file_path, "w") as file:
    file.write(file_contents)

print("compile_commands.json updated.")
