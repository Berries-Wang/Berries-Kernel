# 错误处理


## 000.用panic!处理不可恢复的错误
### 响应 panic 时的栈展开或终止
|行为|说明|备注|
|-|-|-|
|栈展开|- 当出现 panic 时，程序默认会开始 展开（unwinding），这意味着 Rust 会回溯栈并清理它遇到的每一个函数的数据|-|
|-|-|-|
|终止|- 另一种选择是直接 终止（abort），这会不清理数据就退出程序|- panic 时通过在 Cargo.toml 的 [profile] 部分增加 panic = 'abort'，可以由展开切换为终止|
|-|-|-|

--- 

## 001. 用 Result 处理可恢复的错误

### 传播错误
传播（propagating）错误: 当函数的实现中调用了可能会失败的操作时，除了在这个函数中处理错误外，还可以选择让调用者知道这个错误并决定该如何处理。<sub>这样能更好的控制代码调用，因为比起你代码所拥有的上下文，调用者可能拥有更多信息或逻辑来决定应该如何处理错误</sub>

#### 使用 match 将错误传播给调用者
```rust
use std::fs::File;
use std::io::{self, Read};

fn read_username_from_file() -> Result<String, io::Error> {
    let username_file_result = File::open("hello.txt");

    let mut username_file = match username_file_result {
        Ok(file) => file,
        Err(e) => return Err(e),
    };

    let mut username = String::new();

    match username_file.read_to_string(&mut username) {
        Ok(_) => Ok(username),
        Err(e) => Err(e),
    }
}
```

#### 使用 ? 运算符将错误传播给调用者
```rust
use std::fs::File;
use std::io::{self, Read};

fn read_username_from_file() -> Result<String, io::Error> {
    /**
     *? ： 
     *   如果 Result 的值是 Ok，这个表达式将会返回 Ok 中的值而程序将继续执行；
     *   如果值是 Err，Err 将作为整个函数的返回值，就好像使用了 return 关键字一样，这样错误值就被传播给了调用者
     *
     */
    let mut username_file = File::open("hello.txt")?;
    let mut username = String::new();
    username_file.read_to_string(&mut username)?;
    Ok(username)
}

```