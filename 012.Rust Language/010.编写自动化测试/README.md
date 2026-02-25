# 编写自动化测试



## 集成测试
集成测试的目的是测试库的多个部分能否一起正常工作。一些单独能正确运行的代码单元集成在一起也可能会出现问题，所以集成测试的覆盖率也是很重要的。

|概念|说明|备注|使用说明|
|-|-|-|-|
|- tests 目录 (与src同级)|- Cargo 知道如何去寻找这个目录中的集成测试文件。接着可以随意在这个目录中创建任意多的测试文件，Cargo 会将每一个文件当作单独的 crate 来编译。 <br/> - 并不需要将 tests/integration_test.rs 中的任何代码标注为 #[cfg(test)]。 tests 文件夹在 Cargo 中是一个特殊的文件夹，Cargo 只会在运行 cargo test 时编译这个目录中的文件|-|- cargo test --test integration_test <sup>integration_test.rs</sup> ：cargo test 的 --test 后跟文件的名称来运行某个特定集成测试文件中的所有测试|
|-|-|-|-|
|-|-|-|-|
|-|-|-|-|
|-|-|-|-|
|-|-|-|-|