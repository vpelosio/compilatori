; ModuleID = 'AlgebraicIdentityTest.mem2reg.optimized.bc'
source_filename = "AlgebraicIdentityTest.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [22 x i8] c"Result of x+0 is: %d\0A\00", align 1
@.str.1 = private unnamed_addr constant [22 x i8] c"Result of 0+x is: %d\0A\00", align 1
@.str.2 = private unnamed_addr constant [20 x i8] c"Result of a is: %d\0A\00", align 1
@.str.3 = private unnamed_addr constant [23 x i8] c"Result of x+19 is: %d\0A\00", align 1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = add nsw i32 20, 30
  %2 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef 20)
  %3 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, i32 noundef 20)
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %1)
  %5 = add nsw i32 20, 19
  %6 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, i32 noundef %5)
  ret i32 0
}

declare i32 @printf(ptr noundef, ...) #1

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
