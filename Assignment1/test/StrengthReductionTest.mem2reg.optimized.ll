; ModuleID = 'StrengthReductionTest.mem2reg.ll'
source_filename = "StrengthReductionTest.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [6 x i8] c"%d %d\00", align 1
@.str.1 = private unnamed_addr constant [16 x i8] c"%d %d %d %d %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulPowerOf2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  %4 = load i32, ptr %1, align 4
  %5 = shl i32 %4, 4
  %6 = load i32, ptr %1, align 4
  %7 = shl i32 %6, 4
  %8 = load i32, ptr %1, align 4
  %9 = shl i32 %8, 4
  %10 = load i32, ptr %1, align 4
  %11 = load i32, ptr %2, align 4
  %12 = mul nsw i32 %10, %11
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %5, i32 noundef %7, i32 noundef 144, i32 noundef %9, i32 noundef %12)
  ret void
}

declare i32 @__isoc99_scanf(ptr noundef, ...) #1

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulNotPowerOf2() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  %4 = load i32, ptr %1, align 4
  %5 = mul nsw i32 23, %4
  %6 = load i32, ptr %1, align 4
  %7 = mul nsw i32 %6, 23
  %8 = load i32, ptr %1, align 4
  %9 = mul nsw i32 %8, 23
  %10 = load i32, ptr %1, align 4
  %11 = load i32, ptr %2, align 4
  %12 = mul nsw i32 %10, %11
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %5, i32 noundef %7, i32 noundef 207, i32 noundef %9, i32 noundef %12)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulPowerOfTwoMinusOne() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  %4 = load i32, ptr %1, align 4
  %5 = shl i32 %4, 4
  %6 = sub i32 %5, %4
  %7 = load i32, ptr %1, align 4
  %8 = shl i32 %7, 4
  %9 = sub i32 %8, %7
  %10 = load i32, ptr %1, align 4
  %11 = shl i32 %10, 4
  %12 = sub i32 %11, %10
  %13 = load i32, ptr %1, align 4
  %14 = load i32, ptr %2, align 4
  %15 = mul nsw i32 %13, %14
  %16 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %6, i32 noundef %9, i32 noundef 135, i32 noundef %12, i32 noundef %15)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @testMulPowerOfTwoPlusOne() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  %4 = load i32, ptr %1, align 4
  %5 = shl i32 %4, 4
  %6 = add i32 %5, %4
  %7 = load i32, ptr %1, align 4
  %8 = shl i32 %7, 4
  %9 = add i32 %8, %7
  %10 = load i32, ptr %1, align 4
  %11 = shl i32 %10, 4
  %12 = add i32 %11, %10
  %13 = load i32, ptr %1, align 4
  %14 = load i32, ptr %2, align 4
  %15 = mul nsw i32 %13, %14
  %16 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %6, i32 noundef %9, i32 noundef 153, i32 noundef %12, i32 noundef %15)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @testUnsignedDivisionPowerOfTwo() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = call i32 (ptr, ...) @__isoc99_scanf(ptr noundef @.str, ptr noundef %1, ptr noundef %2)
  %4 = load i32, ptr %1, align 4
  %5 = udiv i32 8, %4
  %6 = load i32, ptr %1, align 4
  %7 = lshr i32 %6, 4
  %8 = load i32, ptr %1, align 4
  %9 = lshr i32 %8, 3
  %10 = load i32, ptr %1, align 4
  %11 = load i32, ptr %2, align 4
  %12 = udiv i32 %10, %11
  %13 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef %5, i32 noundef %7, i32 noundef 2, i32 noundef %9, i32 noundef %12)
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
