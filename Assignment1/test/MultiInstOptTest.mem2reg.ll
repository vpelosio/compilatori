; ModuleID = 'MultiInstOptTest.ll'
source_filename = "MultiInstOptTest.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [3 x i8] c"%u\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @testAddMultInstOpt() #0 {
  %1 = alloca i32, align 4
  %2 = call i32 (ptr, ...) @scanf(ptr noundef @.str, ptr noundef %1)
  %3 = load i32, ptr %1, align 4
  %4 = add nsw i32 1, %3
  %5 = add nsw i32 %4, 2
  %6 = sub nsw i32 %5, 9
  %7 = sub nsw i32 %4, 1
  %8 = add nsw i32 %7, 140
  %9 = load i32, ptr %1, align 4
  %10 = add nsw i32 %9, 7
  %11 = add nsw i32 %4, 9
  %12 = sub nsw i32 %5, 2
  %13 = mul nsw i32 %12, 9
  %14 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %7)
  ret void
}

declare i32 @scanf(ptr noundef, ...) #1

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @testSubMultInstOpt() #0 {
  %1 = alloca i32, align 4
  %2 = call i32 (ptr, ...) @scanf(ptr noundef @.str, ptr noundef %1)
  %3 = load i32, ptr %1, align 4
  %4 = sub nsw i32 %3, 1
  %5 = add nsw i32 1, %4
  %6 = add nsw i32 %5, 140
  %7 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %5)
  ret void
}

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @testMulMultInstOpt() #0 {
  %1 = alloca i32, align 4
  %2 = call i32 (ptr, ...) @scanf(ptr noundef @.str.1, ptr noundef %1)
  %3 = load i32, ptr %1, align 4
  %4 = mul i32 %3, 2
  %5 = udiv i32 %4, 2
  %6 = add nsw i32 %5, 140
  %7 = load i32, ptr %1, align 4
  %8 = mul i32 5, %7
  %9 = load i32, ptr %1, align 4
  %10 = udiv i32 5, %9
  %11 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %5)
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
