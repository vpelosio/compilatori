; ModuleID = 'StrengthReductionTest.c'
source_filename = "StrengthReductionTest.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [6 x i8] c"%d %d\00", align 1
@.str.1 = private unnamed_addr constant [16 x i8] c"%d %d %d %d %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulPowerOf2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  store i32 16, ptr %3, align 4
  %10 = load i32, ptr %1, align 4
  %11 = mul nsw i32 16, %10
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %1, align 4
  %13 = mul nsw i32 %12, 16
  store i32 %13, ptr %5, align 4
  store i32 144, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %3, align 4
  %16 = mul nsw i32 %14, %15
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %1, align 4
  %18 = load i32, ptr %2, align 4
  %19 = mul nsw i32 %17, %18
  store i32 %19, ptr %8, align 4
  %20 = load i32, ptr %4, align 4
  %21 = load i32, ptr %5, align 4
  %22 = load i32, ptr %6, align 4
  %23 = load i32, ptr %7, align 4
  %24 = load i32, ptr %8, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %20, i32 noundef %21, i32 noundef %22, i32 noundef %23, i32 noundef %24)
  ret void
}

declare i32 @__isoc99_scanf(ptr noundef, ...) #1

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulNotPowerOf2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  store i32 23, ptr %3, align 4
  %10 = load i32, ptr %1, align 4
  %11 = mul nsw i32 23, %10
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %1, align 4
  %13 = mul nsw i32 %12, 23
  store i32 %13, ptr %5, align 4
  store i32 207, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %3, align 4
  %16 = mul nsw i32 %14, %15
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %1, align 4
  %18 = load i32, ptr %2, align 4
  %19 = mul nsw i32 %17, %18
  store i32 %19, ptr %8, align 4
  %20 = load i32, ptr %4, align 4
  %21 = load i32, ptr %5, align 4
  %22 = load i32, ptr %6, align 4
  %23 = load i32, ptr %7, align 4
  %24 = load i32, ptr %8, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %20, i32 noundef %21, i32 noundef %22, i32 noundef %23, i32 noundef %24)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulPowerOfTwoMinusOne() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  store i32 15, ptr %3, align 4
  %10 = load i32, ptr %1, align 4
  %11 = mul nsw i32 15, %10
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %1, align 4
  %13 = mul nsw i32 %12, 15
  store i32 %13, ptr %5, align 4
  store i32 135, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %3, align 4
  %16 = mul nsw i32 %14, %15
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %1, align 4
  %18 = load i32, ptr %2, align 4
  %19 = mul nsw i32 %17, %18
  store i32 %19, ptr %8, align 4
  %20 = load i32, ptr %4, align 4
  %21 = load i32, ptr %5, align 4
  %22 = load i32, ptr %6, align 4
  %23 = load i32, ptr %7, align 4
  %24 = load i32, ptr %8, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %20, i32 noundef %21, i32 noundef %22, i32 noundef %23, i32 noundef %24)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulPowerOfTwoPlusOne() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  store i32 17, ptr %3, align 4
  %10 = load i32, ptr %1, align 4
  %11 = mul nsw i32 17, %10
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %1, align 4
  %13 = mul nsw i32 %12, 17
  store i32 %13, ptr %5, align 4
  store i32 153, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %3, align 4
  %16 = mul nsw i32 %14, %15
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %1, align 4
  %18 = load i32, ptr %2, align 4
  %19 = mul nsw i32 %17, %18
  store i32 %19, ptr %8, align 4
  %20 = load i32, ptr %4, align 4
  %21 = load i32, ptr %5, align 4
  %22 = load i32, ptr %6, align 4
  %23 = load i32, ptr %7, align 4
  %24 = load i32, ptr %8, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %20, i32 noundef %21, i32 noundef %22, i32 noundef %23, i32 noundef %24)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @testUnsignedDivisionPowerOfTwo() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  store i32 8, ptr %3, align 4
  %10 = load i32, ptr %1, align 4
  %11 = udiv i32 8, %10
  store i32 %11, ptr %4, align 4
  %12 = load i32, ptr %1, align 4
  %13 = udiv i32 %12, 16
  store i32 %13, ptr %5, align 4
  store i32 2, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = load i32, ptr %3, align 4
  %16 = udiv i32 %14, %15
  store i32 %16, ptr %7, align 4
  %17 = load i32, ptr %1, align 4
  %18 = load i32, ptr %2, align 4
  %19 = udiv i32 %17, %18
  store i32 %19, ptr %8, align 4
  %20 = load i32, ptr %4, align 4
  %21 = load i32, ptr %5, align 4
  %22 = load i32, ptr %6, align 4
  %23 = load i32, ptr %7, align 4
  %24 = load i32, ptr %8, align 4
  %25 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %20, i32 noundef %21, i32 noundef %22, i32 noundef %23, i32 noundef %24)
  ret void
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 19.1.7 (++20250114103320+cd708029e0b2-1~exp1~20250114103432.75)"}
