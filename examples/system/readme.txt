== System( コマンドで全てのオプションを使ったサンプルプログラム ==
・SYS.g1m は C.Basic for FX Ver 2.44 build 10 以降で、SYS.g3m は C.Basic for CGVer 1.44 build 10
以降で動作します。
・本プログラムは、C.Basicがインストールできる全ての電卓 - 最も古い fx-9860G OS1.02 から最新の
fx-CG50 OS3.30 - で、そしてエミュレーターで動作します。 

== A sample program for command System( using all options. ==
- SYS,g1m runs on C.Basic for FX Ver 2.44 build 10 or later,SYS.g3m runs on C.Basic for CG 
Ver 1.44 build 10 and later.
- This program can run on all calculators which C.Basic can be installed, oldest fx-9860G OS1.02 through latest fx-CG50 OS3.30, and on Emulator.

 1: 'ProgramMode:GB1
 2: Screen.T:_ClrVram
 3: 0->i:System((-)1)<10=>1->i
 4: Locate 1,i,"C.Basic for FX  v"
 5: System((-)1)>10=>Locate 13,i,"CG"
 6: Locate 18,i,System(0)/100
 7: Switch System((-)1)
 8: Case 0
 9: Locate 1,1+i,"fx_(-)_9860G"
10: Break
11: Case 1
12: Locate 1,1+i,"fx_(-)_9860G Slim"
13: Break
14: Case 2
15: Locate 1,1+i,"fx_(-)_9860GII"
16: Break
17: Case 3
18: Locate 1,1+i,"fx_(-)_9860GII_(-)_2"
19: Break
20: Case 4
21: Locate 1,1+i,"Graph35+EII"
22: Break
23: Case 10
24: Locate 1,1+i,"fx_(-)_CG10"
25: Break
26: Case 20
27: Locate 1,1+i,"fx_(-)_CG20"
28: Break
29: Case 50
30: Locate 1,1+i,"fx_(-)_CG50"
31: Break
32: SwitchEnd
33: System((-)7)=>Locate 1,1,"Emulator"
34: 
35: Locate 16,1+i,"OS .00"
36: Locate 18,1+i,System((-)2)/100
37: Locate 1,3,"CB VRAM"
38: System((-)1)>10=>Locate 1,3,"VRAM   "
39: Locate 12,3,"0x00000000"
40: Locate 14,3,Hex(System(1))
41: Locate 1,4,"TextVRAM"
42: Locate 12,4,"0x00000000"
43: Locate 14,4,Hex(System(2))
44: Locate 1,5,"GraphVRAM"
45: Locate 12,5,"0x00000000"
46: Locate 14,5,Hex(System(3))
47: Locate 1,6,"WorkVRAM"
48: Locate 12,6,"0x00000000"
49: Locate 14,6,Hex(System(4))
50: Locate 1,7,"ClipBoard"
51: Locate 12,7,"0x00000000"
52: Locate 14,7,Hex(System(10))
53: 
54: FkeyMenu(1,"A sample for System(",L,5)
55: FkeyMenu(6,1018)
56: System((-)2)<104=>FkeyMenu(6,"EXIT")
57: While Getkey1<>29:WhileEnd
58: ClrText