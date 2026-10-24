use std::env;
use std::path::PathBuf;
use std::process::Command;

/// Compile a single eBPF C source file to BPF bytecode object
fn compile_ebpf(src: &PathBuf, obj: &PathBuf) {
    let status = Command::new("clang")
        .args([
            "-target", "bpf",
            "-O2",
            "-g",
            "-Wall",
            "-Werror",
            "-I", "src/ebpf",
            "-c", src.to_str().unwrap(),
            "-o", obj.to_str().unwrap(),
        ])
        .status()
        .expect("failed to run clang");

    if !status.success() {
        panic!("clang failed to compile eBPF program: {}", src.display());
    }
}

fn main() {
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());

    // ================================================================
    // 1. TCP SYN fingerprint eBPF program (JA4T)
    // ================================================================
    let syn_src = PathBuf::from("src/ebpf/tcp_options.bpf.c");
    let syn_obj = out_dir.join("tcp_options.bpf.o");

    println!("cargo:rerun-if-changed=src/ebpf/tcp_options.bpf.c");
    println!("cargo:rerun-if-changed=src/ebpf/tcp_options.h");

    compile_ebpf(&syn_src, &syn_obj);
    println!("cargo:rustc-env=EBPF_OBJECT_PATH={}", syn_obj.display());

    // ================================================================
    // 2. TLS Server Hello JA4S eBPF program
    // ================================================================
    let sh_src = PathBuf::from("src/ebpf/tls_server_hello.bpf.c");
    let sh_obj = out_dir.join("tls_server_hello.bpf.o");

    println!("cargo:rerun-if-changed=src/ebpf/tls_server_hello.bpf.c");

    compile_ebpf(&sh_src, &sh_obj);
    println!("cargo:rustc-env=EBPF_SERVER_HELLO_PATH={}", sh_obj.display());
}
