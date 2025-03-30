; ModuleID = 'MultiInstOptTest.c'
source_filename = "MultiInstOptTest.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @testAddMultInstOpt() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = call i32 (ptr, ...) @scanf(ptr noundef @.str, ptr noundef %1)
  %12 = load i32, ptr %1, align 4
  %13 = add nsw i32 %12, 1
  store i32 %13, ptr %2, align 4
  %14 = load i32, ptr %2, align 4
  %15 = add nsw i32 %14, 2
  store i32 %15, ptr %3, align 4
  %16 = load i32, ptr %3, align 4
  %17 = sub nsw i32 %16, 9
  store i32 %17, ptr %4, align 4
  %18 = load i32, ptr %2, align 4
  %19 = sub nsw i32 %18, 1
  store i32 %19, ptr %5, align 4
  %20 = load i32, ptr %5, align 4
  %21 = add nsw i32 %20, 140
  store i32 %21, ptr %6, align 4
  %22 = load i32, ptr %1, align 4
  %23 = add nsw i32 %22, 7
  store i32 %23, ptr %7, align 4
  %24 = load i32, ptr %2, align 4
  %25 = add nsw i32 %24, 9
  store i32 %25, ptr %8, align 4
  %26 = load i32, ptr %3, align 4
  %27 = sub nsw i32 %26, 2
  store i32 %27, ptr %9, align 4
  %28 = load i32, ptr %9, align 4
  %29 = mul nsw i32 %28, 9
  store i32 %29, ptr %10, align 4
  %30 = load i32, ptr %5, align 4
  %31 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %30)
  ret void
}

declare i32 @scanf(ptr noundef, ...) #1

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @testSubMultInstOpt() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = call i32 (ptr, ...) @scanf(ptr noundef @.str, ptr noundef %1)
  %6 = load i32, ptr %1, align 4
  %7 = sub nsw i32 %6, 1
  store i32 %7, ptr %2, align 4
  %8 = load i32, ptr %2, align 4
  %9 = add nsw i32 %8, 1
  store i32 %9, ptr %3, align 4
  %10 = load i32, ptr %3, align 4
  %11 = add nsw i32 %10, 140
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %3, align 4
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %12)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @testMulMultInstOpt() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = call i32 (ptr, ...) @scanf(ptr noundef @.str, ptr noundef %1)
  %8 = load i32, ptr %1, align 4
  %9 = mul nsw i32 %8, 2
  store i32 %9, ptr %2, align 4
  %10 = load i32, ptr %2, align 4
  %11 = sdiv i32 %10, 2
  store i32 %11, ptr %3, align 4
  %12 = load i32, ptr %3, align 4
  %13 = add nsw i32 %12, 140
  store i32 %13, ptr %4, align 4
  %14 = load i32, ptr %1, align 4
  %15 = mul nsw i32 5, %14
  store i32 %15, ptr %5, align 4
  %16 = load i32, ptr %1, align 4
  %17 = sdiv i32 5, %16
  store i32 %17, ptr %6, align 4
  %18 = load i32, ptr %3, align 4
  %19 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %18)
  ret void
}

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"uwtable", i32 1}
!3 = !{i32 7, !"frame-pointer", i32 1}
!4 = !{!"Homebrew clang version 19.1.7"}
