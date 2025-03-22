; ModuleID = 'AlgebraicIdentityTest.c'
source_filename = "AlgebraicIdentityTest.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [18 x i8] c"%d %d %d %d %d %d\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local void @AlgebraicIdentitySumTest() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 20, ptr %1, align 4
  %8 = load i32, ptr %1, align 4
  %9 = add nsw i32 %8, 0
  store i32 %9, ptr %2, align 4
  %10 = load i32, ptr %1, align 4
  %11 = add nsw i32 0, %10
  store i32 %11, ptr %3, align 4
  store i32 15, ptr %4, align 4
  store i32 15, ptr %5, align 4
  %12 = load i32, ptr %1, align 4
  %13 = add nsw i32 %12, 50
  store i32 %13, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = add nsw i32 50, %14
  store i32 %15, ptr %7, align 4
  %16 = load i32, ptr %2, align 4
  %17 = load i32, ptr %3, align 4
  %18 = load i32, ptr %4, align 4
  %19 = load i32, ptr %5, align 4
  %20 = load i32, ptr %6, align 4
  %21 = load i32, ptr %7, align 4
  %22 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %16, i32 noundef %17, i32 noundef %18, i32 noundef %19, i32 noundef %20, i32 noundef %21)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @AlgebraicIdentityTestMul() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 20, ptr %1, align 4
  %8 = load i32, ptr %1, align 4
  %9 = mul nsw i32 %8, 1
  store i32 %9, ptr %2, align 4
  %10 = load i32, ptr %1, align 4
  %11 = mul nsw i32 1, %10
  store i32 %11, ptr %3, align 4
  store i32 12, ptr %4, align 4
  store i32 12, ptr %5, align 4
  %12 = load i32, ptr %1, align 4
  %13 = mul nsw i32 %12, 3
  store i32 %13, ptr %6, align 4
  %14 = load i32, ptr %1, align 4
  %15 = mul nsw i32 3, %14
  store i32 %15, ptr %7, align 4
  %16 = load i32, ptr %2, align 4
  %17 = load i32, ptr %3, align 4
  %18 = load i32, ptr %4, align 4
  %19 = load i32, ptr %5, align 4
  %20 = load i32, ptr %6, align 4
  %21 = load i32, ptr %7, align 4
  %22 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %16, i32 noundef %17, i32 noundef %18, i32 noundef %19, i32 noundef %20, i32 noundef %21)
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
