# 模块系统（the module system）
|意义|说明|
|-|-|
|- 组织代码|- 编写大型的软件系统|
|-|-|
|-|-|

---

## 模块系统（the module system）
模块系统包含以下内容:

|名称|说明|备注|
|-|-|-|
|- 包（Packages）|- Cargo 的一个功能，它允许你构建、测试和分享 crate|- 包（package）是提供一系列功能的一个或者多个 crate 的捆绑 <br/>|
|-|-|-|
|- Crates|- 一个模块的树形结构，它形成了库或可执行文件项目|- crate 是 Rust 在编译时最小的代码单位 <br/>|
|-|-|-|
|- 模块（Modules）和 use|- 允许你控制作用域和路径的私有性|- cargo new restaurant --lib : 创建一个新的名为 restaurant 的库 <br/>|
|-|-|-|
|- 路径（path）|- 一个为例如结构体、函数或模块等项命名的方式|-|
|-|-|-|


## 模块工作原理(how modules work)
|一步步开始|说明|备注|
|-|-|-|
|- 1.|- 从 crate 根节点开始|-|
|- 2.|- 声明模块|-|
|-|-|-|
|-|- 声明子模块|-|
|-|-|-|
|-|- 模块中的代码路径|-|
|-|-|-|
|-|- 私有 vs 公用|-|
|-|-|-|
|-|- use 关键字|-|

### 如何理解 Crates ?
##### 模块树（module tree）/ (crate root)
```rust
// src/lib.rs 叫做 crate 根 （create root） 之所以这样叫它(create root)是因为这两个文件的内容都分别在 crate 模块结构的根组成了一个名为 crate 的模块，该结构被称为模块树（module tree）
mod front_of_house {
    mod hosting {
        fn add_to_waitlist() {}
        fn seat_at_table() {}
    }

    mod serving {
        fn take_order() {}
        fn serve_order() {}
        fn take_payment() {}
    }
}

// 对应的模块树((module tree）)
crate
 └── front_of_house
     ├── hosting
     │   ├── add_to_waitlist
     │   └── seat_at_table
     └── serving
         ├── take_order
         ├── serve_order
         └── take_payment
```

##### 如何理解模块树的项?
“项”（Item） 是构成程序的最小功能单元

 “作用域（scope）”：代码所在的嵌套上下文有一组定义为 “in scope” 的名称。当阅读、编写和编译代码时，程序员和编译器需要知道特定位置的特定名称是否引用了变量、函数、结构体、枚举、模块、常量或者其他有意义的项


###### 模块路径
|名称|使用方式|备注|
|-|-|-|
|- 绝对路径|-|-|
|-|-|-|
|- 相对路径|-|-|
|-|-|-|

