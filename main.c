#include <assert.h> 
#include <ctype.h> 
#include <stdbool.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
 
#include <avr/pgmspace.h> 
 
 
 // This will translate a string pointer into a compressed string in flash
 #define GET_CSTR(x) "J"
 
int ran(int range) { return rand() % range; } 
 
#define SOFT_NL "\n" 
#define SOFT_HYPHEN "-\n" 
#define EMDASH(x) "\xE2\x80\x94"  /* U+2014 "EM DASH" */ 
 
bool pct(int percent) { return (ran(100) < percent); } 
 
/*========== Forward declarations. ======================================== 
 */ 
void dwarves_upset(void); 
void give_up(void); 
void quit(void); 
 
/*========== The vocabulary. ============================================== 
 * This section corresponds to sections 4--17 in Knuth. 
 */ 
 
typedef enum { 
    WordClass_None, WordClass_Motion, WordClass_Object, 
    WordClass_Action, WordClass_Message 
} WordClass; 
 
#define WORD_TEXT_LEN 5 
 
// Stores the test for each word. The corresponding meaning is in the next table 
 
const char word_text_table[][WORD_TEXT_LEN] ={ 
    {'\x72', '\x6f', '\x61', '\x64', '\x00', }, // (  100) 00000 = road 
    { '\x68','\x69','\x6c','\x6c','\x00', }, // (  100) 00001 = hill 
    { '\x65','\x6e','\x74','\x65','\x72', }, // (  101) 00002 = enter 
    { '\x75','\x70','\x73','\x74','\x72', }, // (  102) 00003 = upstr 
    { '\x64','\x6f','\x77','\x6e','\x73', }, // (  103) 00004 = downs 
    { '\x66','\x6f','\x72','\x65','\x73', }, // (  104) 00005 = fores 
    { '\x6f','\x6e','\x77','\x61','\x72', }, // (  105) 00006 = onwar 
    { '\x63','\x6f','\x6e','\x74','\x69', }, // (  105) 00007 = conti 
    { '\x66','\x6f','\x72','\x77','\x61', }, // (  105) 00008 = forwa 
    { '\x62','\x61','\x63','\x6b','\x00', }, // (  106) 00009 = back 
    { '\x72','\x65','\x74','\x75','\x72', }, // (  106) 00010 = retur 
    { '\x72','\x65','\x74','\x72','\x65', }, // (  106) 00011 = retre 
    { '\x76','\x61','\x6c','\x6c','\x65', }, // (  107) 00012 = valle 
    { '\x73','\x74','\x61','\x69','\x72', }, // (  108) 00013 = stair 
    { '\x6f','\x75','\x74','\x73','\x69', }, // (  109) 00014 = outsi 
    { '\x6c','\x65','\x61','\x76','\x65', }, // (  109) 00015 = leave 
    { '\x65','\x78','\x69','\x74','\x00', }, // (  109) 00016 = exit 
    { '\x6f','\x75','\x74','\x00','\x00', }, // (  109) 00017 = out 
    { '\x68','\x6f','\x75','\x73','\x65', }, // (  110) 00018 = house 
    { '\x62','\x75','\x69','\x6c','\x64', }, // (  110) 00019 = build 
    { '\x67','\x75','\x6c','\x6c','\x79', }, // (  111) 00020 = gully 
    { '\x73','\x74','\x72','\x65','\x61', }, // (  112) 00021 = strea 
    { '\x72','\x6f','\x63','\x6b','\x00', }, // (  113) 00022 = rock 
    { '\x62','\x65','\x64','\x00','\x00', }, // (  114) 00023 = bed 
    { '\x63','\x72','\x61','\x77','\x6c', }, // (  115) 00024 = crawl 
    { '\x63','\x6f','\x62','\x62','\x6c', }, // (  116) 00025 = cobbl 
    { '\x69','\x6e','\x00','\x00','\x00', }, // (  117) 00026 = in 
    { '\x69','\x6e','\x73','\x69','\x64', }, // (  117) 00027 = insid 
    { '\x69','\x6e','\x77','\x61','\x72', }, // (  117) 00028 = inwar 
    { '\x73','\x75','\x72','\x66','\x61', }, // (  118) 00029 = surfa 
    { '\x6e','\x75','\x6c','\x6c','\x00', }, // (  119) 00030 = null 
    { '\x6e','\x6f','\x77','\x68','\x65', }, // (  119) 00031 = nowhe 
    { '\x64','\x61','\x72','\x6b','\x00', }, // (  120) 00032 = dark 
    { '\x74','\x75','\x6e','\x6e','\x65', }, // (  121) 00033 = tunne 
    { '\x70','\x61','\x73','\x73','\x61', }, // (  121) 00034 = passa 
    { '\x6c','\x6f','\x77','\x00','\x00', }, // (  122) 00035 = low 
    { '\x63','\x61','\x6e','\x79','\x6f', }, // (  123) 00036 = canyo 
    { '\x61','\x77','\x6b','\x77','\x61', }, // (  124) 00037 = awkwa 
    { '\x67','\x69','\x61','\x6e','\x74', }, // (  125) 00038 = giant 
    { '\x76','\x69','\x65','\x77','\x00', }, // (  126) 00039 = view 
    { '\x61','\x62','\x6f','\x76','\x65', }, // (  127) 00040 = above 
    { '\x75','\x70','\x00','\x00','\x00', }, // (  127) 00041 = up 
    { '\x75','\x70','\x77','\x61','\x72', }, // (  127) 00042 = upwar 
    { '\x75','\x00','\x00','\x00','\x00', }, // (  127) 00043 = u 
    { '\x61','\x73','\x63','\x65','\x6e', }, // (  127) 00044 = ascen 
    { '\x64','\x65','\x73','\x63','\x65', }, // (  128) 00045 = desce 
    { '\x64','\x6f','\x77','\x6e','\x77', }, // (  128) 00046 = downw 
    { '\x64','\x6f','\x77','\x6e','\x00', }, // (  128) 00047 = down 
    { '\x64','\x00','\x00','\x00','\x00', }, // (  128) 00048 = d 
    { '\x70','\x69','\x74','\x00','\x00', }, // (  129) 00049 = pit 
    { '\x6f','\x75','\x74','\x64','\x6f', }, // (  130) 00050 = outdo 
    { '\x63','\x72','\x61','\x63','\x6b', }, // (  131) 00051 = crack 
    { '\x73','\x74','\x65','\x70','\x73', }, // (  132) 00052 = steps 
    { '\x64','\x6f','\x6d','\x65','\x00', }, // (  133) 00053 = dome 
    { '\x6c','\x65','\x66','\x74','\x00', }, // (  134) 00054 = left 
    { '\x72','\x69','\x67','\x68','\x74', }, // (  135) 00055 = right 
    { '\x68','\x61','\x6c','\x6c','\x00', }, // (  136) 00056 = hall 
    { '\x6a','\x75','\x6d','\x70','\x00', }, // (  137) 00057 = jump 
    { '\x62','\x61','\x72','\x72','\x65', }, // (  138) 00058 = barre 
    { '\x6f','\x76','\x65','\x72','\x00', }, // (  139) 00059 = over 
    { '\x61','\x63','\x72','\x6f','\x73', }, // (  140) 00060 = acros 
    { '\x65','\x61','\x73','\x74','\x00', }, // (  141) 00061 = east 
    { '\x65','\x00','\x00','\x00','\x00', }, // (  141) 00062 = e 
    { '\x77','\x65','\x73','\x74','\x00', }, // (  142) 00063 = west 
    { '\x77','\x00','\x00','\x00','\x00', }, // (  142) 00064 = w 
    { '\x6e','\x6f','\x72','\x74','\x68', }, // (  143) 00065 = north 
    { '\x6e','\x00','\x00','\x00','\x00', }, // (  143) 00066 = n 
    { '\x73','\x6f','\x75','\x74','\x68', }, // (  144) 00067 = south 
    { '\x73','\x00','\x00','\x00','\x00', }, // (  144) 00068 = s 
    { '\x6e','\x65','\x00','\x00','\x00', }, // (  145) 00069 = ne 
    { '\x73','\x65','\x00','\x00','\x00', }, // (  146) 00070 = se 
    { '\x73','\x77','\x00','\x00','\x00', }, // (  147) 00071 = sw 
    { '\x6e','\x77','\x00','\x00','\x00', }, // (  148) 00072 = nw 
    { '\x64','\x65','\x62','\x72','\x69', }, // (  149) 00073 = debri 
    { '\x68','\x6f','\x6c','\x65','\x00', }, // (  150) 00074 = hole 
    { '\x77','\x61','\x6c','\x6c','\x00', }, // (  151) 00075 = wall 
    { '\x62','\x72','\x6f','\x6b','\x65', }, // (  152) 00076 = broke 
    { '\x79','\x32','\x00','\x00','\x00', }, // (  153) 00077 = y2 
    { '\x63','\x6c','\x69','\x6d','\x62', }, // (  154) 00078 = climb 
    { '\x74','\x6f','\x75','\x63','\x68', }, // (  155) 00079 = touch 
    { '\x64','\x65','\x73','\x63','\x72', }, // (  155) 00080 = descr 
    { '\x6c','\x6f','\x6f','\x6b','\x00', }, // (  155) 00081 = look 
    { '\x65','\x78','\x61','\x6d','\x69', }, // (  155) 00082 = exami 
    { '\x66','\x6c','\x6f','\x6f','\x72', }, // (  156) 00083 = floor 
    { '\x72','\x6f','\x6f','\x6d','\x00', }, // (  157) 00084 = room 
    { '\x73','\x6c','\x69','\x74','\x00', }, // (  158) 00085 = slit 
    { '\x73','\x6c','\x61','\x62','\x72', }, // (  159) 00086 = slabr 
    { '\x73','\x6c','\x61','\x62','\x00', }, // (  159) 00087 = slab 
    { '\x78','\x79','\x7a','\x7a','\x79', }, // (  160) 00088 = xyzzy 
    { '\x64','\x65','\x70','\x72','\x65', }, // (  161) 00089 = depre 
    { '\x65','\x6e','\x74','\x72','\x61', }, // (  162) 00090 = entra 
    { '\x70','\x6c','\x75','\x67','\x68', }, // (  163) 00091 = plugh 
    { '\x73','\x65','\x63','\x72','\x65', }, // (  164) 00092 = secre 
    { '\x63','\x61','\x76','\x65','\x00', }, // (  165) 00093 = cave 
    { '\x63','\x72','\x6f','\x73','\x73', }, // (  166) 00094 = cross 
    { '\x62','\x65','\x64','\x71','\x75', }, // (  167) 00095 = bedqu 
    { '\x70','\x6c','\x6f','\x76','\x65', }, // (  168) 00096 = plove 
    { '\x6f','\x72','\x69','\x65','\x6e', }, // (  169) 00097 = orien 
    { '\x63','\x61','\x76','\x65','\x72', }, // (  170) 00098 = caver 
    { '\x73','\x68','\x65','\x6c','\x6c', }, // (  171) 00099 = shell 
    { '\x72','\x65','\x73','\x65','\x72', }, // (  172) 00100 = reser 
    { '\x6f','\x66','\x66','\x69','\x63', }, // (  173) 00101 = offic 
    { '\x6d','\x61','\x69','\x6e','\x00', }, // (  173) 00102 = main 
    { '\x66','\x6f','\x72','\x6b','\x00', }, // (  174) 00103 = fork 
    { '\x6b','\x65','\x79','\x73','\x00', }, // (  200) 00104 = keys 
    { '\x6b','\x65','\x79','\x00','\x00', }, // (  200) 00105 = key 
    { '\x6c','\x61','\x6d','\x70','\x00', }, // (  201) 00106 = lamp 
    { '\x6c','\x61','\x6e','\x74','\x65', }, // (  201) 00107 = lante 
    { '\x68','\x65','\x61','\x64','\x6c', }, // (  201) 00108 = headl 
    { '\x67','\x72','\x61','\x74','\x65', }, // (  202) 00109 = grate 
    { '\x63','\x61','\x67','\x65','\x00', }, // (  204) 00110 = cage 
    { '\x72','\x6f','\x64','\x00','\x00', }, // (  205) 00111 = rod 
    { '\x62','\x69','\x72','\x64','\x00', }, // (  209) 00112 = bird 
    { '\x64','\x6f','\x6f','\x72','\x00', }, // (  210) 00113 = door 
    { '\x70','\x69','\x6c','\x6c','\x6f', }, // (  211) 00114 = pillo 
    { '\x76','\x65','\x6c','\x76','\x65', }, // (  211) 00115 = velve 
    { '\x73','\x6e','\x61','\x6b','\x65', }, // (  212) 00116 = snake 
    { '\x66','\x69','\x73','\x73','\x75', }, // (  213) 00117 = fissu 
    { '\x74','\x61','\x62','\x6c','\x65', }, // (  215) 00118 = table 
    { '\x63','\x6c','\x61','\x6d','\x00', }, // (  216) 00119 = clam 
    { '\x6f','\x79','\x73','\x74','\x65', }, // (  217) 00120 = oyste 
    { '\"','\x73','\x70','\x65','\x6c', }, // (  218) 00121 = "spel 
    { '\x73','\x70','\x65','\x6c','\x75', }, // (  218) 00122 = spelu 
    { '\x69','\x73','\x73','\x75','\x65', }, // (  218) 00123 = issue 
    { '\x6d','\x61','\x67','\x61','\x7a', }, // (  218) 00124 = magaz 
    { '\x64','\x77','\x61','\x72','\x66', }, // (  219) 00125 = dwarf 
    { '\x64','\x77','\x61','\x72','\x76', }, // (  219) 00126 = dwarv 
    { '\x6b','\x6e','\x69','\x66','\x65', }, // (  220) 00127 = knife 
    { '\x6b','\x6e','\x69','\x76','\x65', }, // (  220) 00128 = knive 
    { '\x72','\x61','\x74','\x69','\x6f', }, // (  221) 00129 = ratio 
    { '\x66','\x6f','\x6f','\x64','\x00', }, // (  221) 00130 = food 
    { '\x62','\x6f','\x74','\x74','\x6c', }, // (  222) 00131 = bottl 
    { '\x6a','\x61','\x72','\x00','\x00', }, // (  222) 00132 = jar 
    { '\x68','\x32','\x6f','\x00','\x00', }, // (  223) 00133 = h2o 
    { '\x77','\x61','\x74','\x65','\x72', }, // (  223) 00134 = water 
    { '\x6f','\x69','\x6c','\x00','\x00', }, // (  224) 00135 = oil 
    { '\x6d','\x69','\x72','\x72','\x6f', }, // (  225) 00136 = mirro 
    { '\x70','\x6c','\x61','\x6e','\x74', }, // (  227) 00137 = plant 
    { '\x62','\x65','\x61','\x6e','\x73', }, // (  227) 00138 = beans 
    { '\x73','\x74','\x61','\x6c','\x61', }, // (  230) 00139 = stala 
    { '\x66','\x69','\x67','\x75','\x72', }, // (  231) 00140 = figur 
    { '\x73','\x68','\x61','\x64','\x6f', }, // (  231) 00141 = shado 
    { '\x61','\x78','\x65','\x00','\x00', }, // (  233) 00142 = axe 
    { '\x64','\x72','\x61','\x77','\x69', }, // (  234) 00143 = drawi 
    { '\x70','\x69','\x72','\x61','\x74', }, // (  235) 00144 = pirat 
    { '\x64','\x72','\x61','\x67','\x6f', }, // (  236) 00145 = drago 
    { '\x63','\x68','\x61','\x73','\x6d', }, // (  238) 00146 = chasm 
    { '\x74','\x72','\x6f','\x6c','\x6c', }, // (  240) 00147 = troll 
    { '\x62','\x65','\x61','\x72','\x00', }, // (  244) 00148 = bear 
    { '\x6d','\x65','\x73','\x73','\x61', }, // (  245) 00149 = messa 
    { '\x76','\x6f','\x6c','\x63','\x61', }, // (  246) 00150 = volca 
    { '\x67','\x65','\x79','\x73','\x65', }, // (  246) 00151 = geyse 
    { '\x76','\x65','\x6e','\x64','\x69', }, // (  247) 00152 = vendi 
    { '\x6d','\x61','\x63','\x68','\x69', }, // (  247) 00153 = machi 
    { '\x62','\x61','\x74','\x74','\x65', }, // (  248) 00154 = batte 
    { '\x63','\x61','\x72','\x70','\x65', }, // (  249) 00155 = carpe 
    { '\x6d','\x6f','\x73','\x73','\x00', }, // (  249) 00156 = moss 
    { '\x67','\x6f','\x6c','\x64','\x00', }, // (  250) 00157 = gold 
    { '\x6e','\x75','\x67','\x67','\x65', }, // (  250) 00158 = nugge 
    { '\x64','\x69','\x61','\x6d','\x6f', }, // (  251) 00159 = diamo 
    { '\x73','\x69','\x6c','\x76','\x65', }, // (  252) 00160 = silve 
    { '\x62','\x61','\x72','\x73','\x00', }, // (  252) 00161 = bars 
    { '\x6a','\x65','\x77','\x65','\x6c', }, // (  253) 00162 = jewel 
    { '\x63','\x6f','\x69','\x6e','\x73', }, // (  254) 00163 = coins 
    { '\x63','\x68','\x65','\x73','\x74', }, // (  255) 00164 = chest 
    { '\x62','\x6f','\x78','\x00','\x00', }, // (  255) 00165 = box 
    { '\x74','\x72','\x65','\x61','\x73', }, // (  255) 00166 = treas 
    { '\x65','\x67','\x67','\x73','\x00', }, // (  256) 00167 = eggs 
    { '\x6e','\x65','\x73','\x74','\x00', }, // (  256) 00168 = nest 
    { '\x65','\x67','\x67','\x00','\x00', }, // (  256) 00169 = egg 
    { '\x74','\x72','\x69','\x64','\x65', }, // (  257) 00170 = tride 
    { '\x76','\x61','\x73','\x65','\x00', }, // (  258) 00171 = vase 
    { '\x73','\x68','\x61','\x72','\x64', }, // (  258) 00172 = shard 
    { '\x70','\x6f','\x74','\x74','\x65', }, // (  258) 00173 = potte 
    { '\x6d','\x69','\x6e','\x67','\x00', }, // (  258) 00174 = ming 
    { '\x65','\x6d','\x65','\x72','\x61', }, // (  259) 00175 = emera 
    { '\x70','\x6c','\x61','\x74','\x69', }, // (  260) 00176 = plati 
    { '\x70','\x79','\x72','\x61','\x6d', }, // (  260) 00177 = pyram 
    { '\x70','\x65','\x61','\x72','\x6c', }, // (  261) 00178 = pearl 
    { '\x72','\x75','\x67','\x00','\x00', }, // (  262) 00179 = rug 
    { '\x70','\x65','\x72','\x73','\x69', }, // (  262) 00180 = persi 
    { '\x73','\x70','\x69','\x63','\x65', }, // (  264) 00181 = spice 
    { '\x63','\x68','\x61','\x69','\x6e', }, // (  265) 00182 = chain 
    { '\x74','\x61','\x6b','\x65','\x00', }, // (  300) 00183 = take 
    { '\x67','\x65','\x74','\x00','\x00', }, // (  300) 00184 = get 
    { '\x63','\x61','\x74','\x63','\x68', }, // (  300) 00185 = catch 
    { '\x6b','\x65','\x65','\x70','\x00', }, // (  300) 00186 = keep 
    { '\x63','\x61','\x72','\x72','\x79', }, // (  300) 00187 = carry 
    { '\x63','\x61','\x70','\x74','\x75', }, // (  300) 00188 = captu 
    { '\x73','\x74','\x65','\x61','\x6c', }, // (  300) 00189 = steal 
    { '\x74','\x6f','\x74','\x65','\x00', }, // (  300) 00190 = tote 
    { '\x64','\x69','\x73','\x63','\x61', }, // (  301) 00191 = disca 
    { '\x72','\x65','\x6c','\x65','\x61', }, // (  301) 00192 = relea 
    { '\x64','\x75','\x6d','\x70','\x00', }, // (  301) 00193 = dump 
    { '\x66','\x72','\x65','\x65','\x00', }, // (  301) 00194 = free 
    { '\x64','\x72','\x6f','\x70','\x00', }, // (  301) 00195 = drop 
    { '\x75','\x6e','\x6c','\x6f','\x63', }, // (  302) 00196 = unloc 
    { '\x6f','\x70','\x65','\x6e','\x00', }, // (  302) 00197 = open 
    { '\x6c','\x6f','\x63','\x6b','\x00', }, // (  303) 00198 = lock 
    { '\x63','\x6c','\x6f','\x73','\x65', }, // (  303) 00199 = close 
    { '\x6f','\x6e','\x00','\x00','\x00', }, // (  304) 00200 = on 
    { '\x6c','\x69','\x67','\x68','\x74', }, // (  304) 00201 = light 
    { '\x6f','\x66','\x66','\x00','\x00', }, // (  305) 00202 = off 
    { '\x65','\x78','\x74','\x69','\x6e', }, // (  305) 00203 = extin 
    { '\x73','\x68','\x61','\x6b','\x65', }, // (  306) 00204 = shake 
    { '\x73','\x77','\x69','\x6e','\x67', }, // (  306) 00205 = swing 
    { '\x77','\x61','\x76','\x65','\x00', }, // (  306) 00206 = wave 
    { '\x63','\x61','\x6c','\x6d','\x00', }, // (  307) 00207 = calm 
    { '\x74','\x61','\x6d','\x65','\x00', }, // (  307) 00208 = tame 
    { '\x70','\x6c','\x61','\x63','\x61', }, // (  307) 00209 = placa 
    { '\x77','\x61','\x6c','\x6b','\x00', }, // (  308) 00210 = walk 
    { '\x72','\x75','\x6e','\x00','\x00', }, // (  308) 00211 = run 
    { '\x70','\x72','\x6f','\x63','\x65', }, // (  308) 00212 = proce 
    { '\x74','\x72','\x61','\x76','\x65', }, // (  308) 00213 = trave 
    { '\x65','\x78','\x70','\x6c','\x6f', }, // (  308) 00214 = explo 
    { '\x66','\x6f','\x6c','\x6c','\x6f', }, // (  308) 00215 = follo 
    { '\x74','\x75','\x72','\x6e','\x00', }, // (  308) 00216 = turn 
    { '\x67','\x6f','\x74','\x6f','\x00', }, // (  308) 00217 = goto 
    { '\x67','\x6f','\x00','\x00','\x00', }, // (  308) 00218 = go 
    { '\x6e','\x6f','\x74','\x68','\x69', }, // (  309) 00219 = nothi 
    { '\x70','\x6f','\x75','\x72','\x00', }, // (  310) 00220 = pour 
    { '\x64','\x65','\x76','\x6f','\x75', }, // (  311) 00221 = devou 
    { '\x65','\x61','\x74','\x00','\x00', }, // (  311) 00222 = eat 
    { '\x64','\x72','\x69','\x6e','\x6b', }, // (  312) 00223 = drink 
    { '\x72','\x75','\x62','\x00','\x00', }, // (  313) 00224 = rub 
    { '\x74','\x68','\x72','\x6f','\x77', }, // (  314) 00225 = throw 
    { '\x74','\x6f','\x73','\x73','\x00', }, // (  314) 00226 = toss 
    { '\x64','\x69','\x73','\x74','\x75', }, // (  315) 00227 = distu 
    { '\x77','\x61','\x6b','\x65','\x00', }, // (  315) 00228 = wake 
    { '\x66','\x65','\x65','\x64','\x00', }, // (  316) 00229 = feed 
    { '\x66','\x69','\x6c','\x6c','\x00', }, // (  317) 00230 = fill 
    { '\x73','\x68','\x61','\x74','\x74', }, // (  318) 00231 = shatt 
    { '\x73','\x6d','\x61','\x73','\x68', }, // (  318) 00232 = smash 
    { '\x62','\x72','\x65','\x61','\x6b', }, // (  318) 00233 = break 
    { '\x69','\x67','\x6e','\x69','\x74', }, // (  319) 00234 = ignit 
    { '\x62','\x6c','\x61','\x73','\x74', }, // (  319) 00235 = blast 
    { '\x64','\x65','\x74','\x6f','\x6e', }, // (  319) 00236 = deton 
    { '\x62','\x6c','\x6f','\x77','\x75', }, // (  319) 00237 = blowu 
    { '\x61','\x74','\x74','\x61','\x63', }, // (  320) 00238 = attac 
    { '\x68','\x69','\x74','\x00','\x00', }, // (  320) 00239 = hit 
    { '\x73','\x74','\x72','\x69','\x6b', }, // (  320) 00240 = strik 
    { '\x66','\x69','\x67','\x68','\x74', }, // (  320) 00241 = fight 
    { '\x6b','\x69','\x6c','\x6c','\x00', }, // (  320) 00242 = kill 
    { '\x73','\x69','\x6e','\x67','\x00', }, // (  321) 00243 = sing 
    { '\x73','\x61','\x79','\x00','\x00', }, // (  321) 00244 = say 
    { '\x63','\x68','\x61','\x6e','\x74', }, // (  321) 00245 = chant 
    { '\x6d','\x75','\x6d','\x62','\x6c', }, // (  321) 00246 = mumbl 
    { '\x75','\x74','\x74','\x65','\x72', }, // (  321) 00247 = utter 
    { '\x70','\x65','\x72','\x75','\x73', }, // (  322) 00248 = perus 
    { '\x72','\x65','\x61','\x64','\x00', }, // (  322) 00249 = read 
    { '\x66','\x69','\x65','\x00','\x00', }, // (  323) 00250 = fie 
    { '\x66','\x6f','\x65','\x00','\x00', }, // (  323) 00251 = foe 
    { '\x66','\x75','\x6d','\x00','\x00', }, // (  323) 00252 = fum 
    { '\x66','\x6f','\x6f','\x00','\x00', }, // (  323) 00253 = foo 
    { '\x66','\x65','\x65','\x00','\x00', }, // (  323) 00254 = fee 
    { '\x62','\x72','\x69','\x65','\x66', }, // (  324) 00255 = brief 
    { '\x77','\x68','\x65','\x72','\x65', }, // (  325) 00256 = where 
    { '\x66','\x69','\x6e','\x64','\x00', }, // (  325) 00257 = find 
    { '\x69','\x6e','\x76','\x65','\x6e', }, // (  326) 00258 = inven 
    { '\x73','\x63','\x6f','\x72','\x65', }, // (  327) 00259 = score 
    { '\x71','\x75','\x69','\x74','\x00', }, // (  328) 00260 = quit 
    { '\x70','\x6f','\x63','\x75','\x73', }, // (  400) 00261 = pocus 
    { '\x61','\x62','\x72','\x61','\x63', }, // (  400) 00262 = abrac 
    { '\x6f','\x70','\x65','\x6e','\x73', }, // (  400) 00263 = opens 
    { '\x73','\x68','\x61','\x7a','\x61', }, // (  400) 00264 = shaza 
    { '\x68','\x6f','\x63','\x75','\x73', }, // (  400) 00265 = hocus 
    { '\x61','\x62','\x72','\x61','\x00', }, // (  400) 00266 = abra 
    { '\x73','\x65','\x73','\x61','\x6d', }, // (  400) 00267 = sesam 
    { '\x3f','\x00','\x00','\x00','\x00', }, // (  401) 00268 = ? 
    { '\x68','\x65','\x6c','\x70','\x00', }, // (  401) 00269 = help 
    { '\x74','\x72','\x65','\x65','\x73', }, // (  402) 00270 = trees 
    { '\x74','\x72','\x65','\x65','\x00', }, // (  402) 00271 = tree 
    { '\x65','\x78','\x63','\x61','\x76', }, // (  403) 00272 = excav 
    { '\x64','\x69','\x67','\x00','\x00', }, // (  403) 00273 = dig 
    { '\x6c','\x6f','\x73','\x74','\x00', }, // (  404) 00274 = lost 
    { '\x6d','\x69','\x73','\x74','\x00', }, // (  405) 00275 = mist 
    { '\x66','\x75','\x63','\x6b','\x00', }, // (  406) 00276 = fuck 
    { '\x73','\x74','\x6f','\x70','\x00', }, // (  407) 00277 = stop 
    { '\x69','\x6e','\x66','\x6f','\x72', }, // (  408) 00278 = infor 
    { '\x69','\x6e','\x66','\x6f','\x00', }, // (  408) 00279 = info 
    { '\x73','\x77','\x69','\x6d','\x00', }, // (  409) 00280 = swim 
}; 
 
// Stores the offset from the meaning of the previous record. 
 
const unsigned char word_meaning_offset_table[] = { 
 
    100, // (    0) 00100 = road 
    0, // (    1) 00100 = hill 
    1, // (    2) 00101 = enter 
    1, // (    3) 00102 = upstr 
    1, // (    4) 00103 = downs 
    1, // (    5) 00104 = fores 
    1, // (    6) 00105 = onwar 
    0, // (    7) 00105 = conti 
    0, // (    8) 00105 = forwa 
    1, // (    9) 00106 = back 
    0, // (   10) 00106 = retur 
    0, // (   11) 00106 = retre 
    1, // (   12) 00107 = valle 
    1, // (   13) 00108 = stair 
    1, // (   14) 00109 = outsi 
    0, // (   15) 00109 = leave 
    0, // (   16) 00109 = exit 
    0, // (   17) 00109 = out 
    1, // (   18) 00110 = house 
    0, // (   19) 00110 = build 
    1, // (   20) 00111 = gully 
    1, // (   21) 00112 = strea 
    1, // (   22) 00113 = rock 
    1, // (   23) 00114 = bed 
    1, // (   24) 00115 = crawl 
    1, // (   25) 00116 = cobbl 
    1, // (   26) 00117 = in 
    0, // (   27) 00117 = insid 
    0, // (   28) 00117 = inwar 
    1, // (   29) 00118 = surfa 
    1, // (   30) 00119 = null 
    0, // (   31) 00119 = nowhe 
    1, // (   32) 00120 = dark 
    1, // (   33) 00121 = tunne 
    0, // (   34) 00121 = passa 
    1, // (   35) 00122 = low 
    1, // (   36) 00123 = canyo 
    1, // (   37) 00124 = awkwa 
    1, // (   38) 00125 = giant 
    1, // (   39) 00126 = view 
    1, // (   40) 00127 = above 
    0, // (   41) 00127 = up 
    0, // (   42) 00127 = upwar 
    0, // (   43) 00127 = u 
    0, // (   44) 00127 = ascen 
    1, // (   45) 00128 = desce 
    0, // (   46) 00128 = downw 
    0, // (   47) 00128 = down 
    0, // (   48) 00128 = d 
    1, // (   49) 00129 = pit 
    1, // (   50) 00130 = outdo 
    1, // (   51) 00131 = crack 
    1, // (   52) 00132 = steps 
    1, // (   53) 00133 = dome 
    1, // (   54) 00134 = left 
    1, // (   55) 00135 = right 
    1, // (   56) 00136 = hall 
    1, // (   57) 00137 = jump 
    1, // (   58) 00138 = barre 
    1, // (   59) 00139 = over 
    1, // (   60) 00140 = acros 
    1, // (   61) 00141 = east 
    0, // (   62) 00141 = e 
    1, // (   63) 00142 = west 
    0, // (   64) 00142 = w 
    1, // (   65) 00143 = north 
    0, // (   66) 00143 = n 
    1, // (   67) 00144 = south 
    0, // (   68) 00144 = s 
    1, // (   69) 00145 = ne 
    1, // (   70) 00146 = se 
    1, // (   71) 00147 = sw 
    1, // (   72) 00148 = nw 
    1, // (   73) 00149 = debri 
    1, // (   74) 00150 = hole 
    1, // (   75) 00151 = wall 
    1, // (   76) 00152 = broke 
    1, // (   77) 00153 = y2 
    1, // (   78) 00154 = climb 
    1, // (   79) 00155 = touch 
    0, // (   80) 00155 = descr 
    0, // (   81) 00155 = look 
    0, // (   82) 00155 = exami 
    1, // (   83) 00156 = floor 
    1, // (   84) 00157 = room 
    1, // (   85) 00158 = slit 
    1, // (   86) 00159 = slabr 
    0, // (   87) 00159 = slab 
    1, // (   88) 00160 = xyzzy 
    1, // (   89) 00161 = depre 
    1, // (   90) 00162 = entra 
    1, // (   91) 00163 = plugh 
    1, // (   92) 00164 = secre 
    1, // (   93) 00165 = cave 
    1, // (   94) 00166 = cross 
    1, // (   95) 00167 = bedqu 
    1, // (   96) 00168 = plove 
    1, // (   97) 00169 = orien 
    1, // (   98) 00170 = caver 
    1, // (   99) 00171 = shell 
    1, // (  100) 00172 = reser 
    1, // (  101) 00173 = offic 
    0, // (  102) 00173 = main 
    1, // (  103) 00174 = fork 
    26, // (  104) 00200 = keys 
    0, // (  105) 00200 = key 
    1, // (  106) 00201 = lamp 
    0, // (  107) 00201 = lante 
    0, // (  108) 00201 = headl 
    1, // (  109) 00202 = grate 
    2, // (  110) 00204 = cage 
    1, // (  111) 00205 = rod 
    4, // (  112) 00209 = bird 
    1, // (  113) 00210 = door 
    1, // (  114) 00211 = pillo 
    0, // (  115) 00211 = velve 
    1, // (  116) 00212 = snake 
    1, // (  117) 00213 = fissu 
    2, // (  118) 00215 = table 
    1, // (  119) 00216 = clam 
    1, // (  120) 00217 = oyste 
    1, // (  121) 00218 = "spel 
    0, // (  122) 00218 = spelu 
    0, // (  123) 00218 = issue 
    0, // (  124) 00218 = magaz 
    1, // (  125) 00219 = dwarf 
    0, // (  126) 00219 = dwarv 
    1, // (  127) 00220 = knife 
    0, // (  128) 00220 = knive 
    1, // (  129) 00221 = ratio 
    0, // (  130) 00221 = food 
    1, // (  131) 00222 = bottl 
    0, // (  132) 00222 = jar 
    1, // (  133) 00223 = h2o 
    0, // (  134) 00223 = water 
    1, // (  135) 00224 = oil 
    1, // (  136) 00225 = mirro 
    2, // (  137) 00227 = plant 
    0, // (  138) 00227 = beans 
    3, // (  139) 00230 = stala 
    1, // (  140) 00231 = figur 
    0, // (  141) 00231 = shado 
    2, // (  142) 00233 = axe 
    1, // (  143) 00234 = drawi 
    1, // (  144) 00235 = pirat 
    1, // (  145) 00236 = drago 
    2, // (  146) 00238 = chasm 
    2, // (  147) 00240 = troll 
    4, // (  148) 00244 = bear 
    1, // (  149) 00245 = messa 
    1, // (  150) 00246 = volca 
    0, // (  151) 00246 = geyse 
    1, // (  152) 00247 = vendi 
    0, // (  153) 00247 = machi 
    1, // (  154) 00248 = batte 
    1, // (  155) 00249 = carpe 
    0, // (  156) 00249 = moss 
    1, // (  157) 00250 = gold 
    0, // (  158) 00250 = nugge 
    1, // (  159) 00251 = diamo 
    1, // (  160) 00252 = silve 
    0, // (  161) 00252 = bars 
    1, // (  162) 00253 = jewel 
    1, // (  163) 00254 = coins 
    1, // (  164) 00255 = chest 
    0, // (  165) 00255 = box 
    0, // (  166) 00255 = treas 
    1, // (  167) 00256 = eggs 
    0, // (  168) 00256 = nest 
    0, // (  169) 00256 = egg 
    1, // (  170) 00257 = tride 
    1, // (  171) 00258 = vase 
    0, // (  172) 00258 = shard 
    0, // (  173) 00258 = potte 
    0, // (  174) 00258 = ming 
    1, // (  175) 00259 = emera 
    1, // (  176) 00260 = plati 
    0, // (  177) 00260 = pyram 
    1, // (  178) 00261 = pearl 
    1, // (  179) 00262 = rug 
    0, // (  180) 00262 = persi 
    2, // (  181) 00264 = spice 
    1, // (  182) 00265 = chain 
    35, // (  183) 00300 = take 
    0, // (  184) 00300 = get 
    0, // (  185) 00300 = catch 
    0, // (  186) 00300 = keep 
    0, // (  187) 00300 = carry 
    0, // (  188) 00300 = captu 
    0, // (  189) 00300 = steal 
    0, // (  190) 00300 = tote 
    1, // (  191) 00301 = disca 
    0, // (  192) 00301 = relea 
    0, // (  193) 00301 = dump 
    0, // (  194) 00301 = free 
    0, // (  195) 00301 = drop 
    1, // (  196) 00302 = unloc 
    0, // (  197) 00302 = open 
    1, // (  198) 00303 = lock 
    0, // (  199) 00303 = close 
    1, // (  200) 00304 = on 
    0, // (  201) 00304 = light 
    1, // (  202) 00305 = off 
    0, // (  203) 00305 = extin 
    1, // (  204) 00306 = shake 
    0, // (  205) 00306 = swing 
    0, // (  206) 00306 = wave 
    1, // (  207) 00307 = calm 
    0, // (  208) 00307 = tame 
    0, // (  209) 00307 = placa 
    1, // (  210) 00308 = walk 
    0, // (  211) 00308 = run 
    0, // (  212) 00308 = proce 
    0, // (  213) 00308 = trave 
    0, // (  214) 00308 = explo 
    0, // (  215) 00308 = follo 
    0, // (  216) 00308 = turn 
    0, // (  217) 00308 = goto 
    0, // (  218) 00308 = go 
    1, // (  219) 00309 = nothi 
    1, // (  220) 00310 = pour 
    1, // (  221) 00311 = devou 
    0, // (  222) 00311 = eat 
    1, // (  223) 00312 = drink 
    1, // (  224) 00313 = rub 
    1, // (  225) 00314 = throw 
    0, // (  226) 00314 = toss 
    1, // (  227) 00315 = distu 
    0, // (  228) 00315 = wake 
    1, // (  229) 00316 = feed 
    1, // (  230) 00317 = fill 
    1, // (  231) 00318 = shatt 
    0, // (  232) 00318 = smash 
    0, // (  233) 00318 = break 
    1, // (  234) 00319 = ignit 
    0, // (  235) 00319 = blast 
    0, // (  236) 00319 = deton 
    0, // (  237) 00319 = blowu 
    1, // (  238) 00320 = attac 
    0, // (  239) 00320 = hit 
    0, // (  240) 00320 = strik 
    0, // (  241) 00320 = fight 
    0, // (  242) 00320 = kill 
    1, // (  243) 00321 = sing 
    0, // (  244) 00321 = say 
    0, // (  245) 00321 = chant 
    0, // (  246) 00321 = mumbl 
    0, // (  247) 00321 = utter 
    1, // (  248) 00322 = perus 
    0, // (  249) 00322 = read 
    1, // (  250) 00323 = fie 
    0, // (  251) 00323 = foe 
    0, // (  252) 00323 = fum 
    0, // (  253) 00323 = foo 
    0, // (  254) 00323 = fee 
    1, // (  255) 00324 = brief 
    1, // (  256) 00325 = where 
    0, // (  257) 00325 = find 
    1, // (  258) 00326 = inven 
    1, // (  259) 00327 = score 
    1, // (  260) 00328 = quit 
    72, // (  261) 00400 = pocus 
    0, // (  262) 00400 = abrac 
    0, // (  263) 00400 = opens 
    0, // (  264) 00400 = shaza 
    0, // (  265) 00400 = hocus 
    0, // (  266) 00400 = abra 
    0, // (  267) 00400 = sesam 
    1, // (  268) 00401 = ? 
    0, // (  269) 00401 = help 
    1, // (  270) 00402 = trees 
    0, // (  271) 00402 = tree 
    1, // (  272) 00403 = excav 
    0, // (  273) 00403 = dig 
    1, // (  274) 00404 = lost 
    1, // (  275) 00405 = mist 
    1, // (  276) 00406 = fuck 
    1, // (  277) 00407 = stop 
    1, // (  278) 00408 = infor 
    0, // (  279) 00408 = info 
    1, // (  280) 00409 = swim 
}; 
 
int streq( const char *a , const char *b) { 
 
    return !strncmp(a, b , WORD_TEXT_LEN); 
 
} 
 
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0])) 
 
int lookup(const char *w) 
{ 
 
    int meaning = 0; 
 
    for (int h = 0; h<ARRAY_LENGTH(word_text_table); h++) { 
 
        meaning += word_meaning_offset_table[h];				// Accumuate the offsets here. We do this to avoid needing to store a full int inside the table. 
 
        if (streq(w, word_text_table[h]) ) return meaning; 
    } 
 
    return 0; 
} 
 
#define NOTHING 0 
 
// This gets defined in PRGMEM and messes up our SE 
 
#undef SE 
 
typedef enum { 
    MIN_MOTION=100, 
    ROAD=MIN_MOTION,ENTER,UPSTREAM,DOWNSTREAM,FOREST,FORWARD, 
    BACK,VALLEY,STAIRS,OUT,HOUSE,GULLY,STREAM,ROCK, 
    BED,CRAWL,COBBLES,IN,SURFACE,NOWHERE,DARK,PASSAGE, 
    LOW,CANYON,AWKWARD,GIANT,VIEW,U,D,PIT,OUTDOORS, 
    CRACK,STEPS,DOME,LEFT,RIGHT,HALL,JUMP,BARREN, 
    OVER,ACROSS,E,W,N,S,NE,SE,SW,NW,DEBRIS,HOLE, 
    WALL,BROKEN,Y2,CLIMB,LOOK,FLOOR,ROOM,SLIT, 
    SLAB,XYZZY,DEPRESSION,ENTRANCE,PLUGH,SECRET, 
    CAVE,CROSS,BEDQUILT,PLOVER,ORIENTAL,CAVERN, 
    SHELL,RESERVOIR,OFFICE,FORK, 
    MAX_MOTION=FORK 
} MotionWord; 
 
typedef enum { 
    MIN_OBJ=200, 
    KEYS=MIN_OBJ, LAMP, GRATE, GRATE_, CAGE, ROD, ROD2, TREADS, TREADS_, 
    BIRD, RUSTY_DOOR, PILLOW, SNAKE, FISSURE, FISSURE_, TABLET, CLAM, OYSTER, 
    MAG, DWARF, KNIFE, FOOD, BOTTLE, WATER, OIL, MIRROR, MIRROR_, PLANT, 
    PLANT2, PLANT2_, STALACTITE, SHADOW, SHADOW_, AXE, DRAWINGS, PIRATE, 
    DRAGON, DRAGON_, CHASM, CHASM_, TROLL, TROLL_, NO_TROLL, NO_TROLL_, 
    BEAR, MESSAGE, GORGE, MACHINE, BATTERIES, MOSS, 
    GOLD, DIAMONDS, SILVER, JEWELS, COINS, CHEST, EGGS, 
    TRIDENT, VASE, EMERALD, PYRAMID, PEARL, RUG, RUG_, SPICES, CHAIN, 
    MAX_OBJ=CHAIN 
} ObjectWord; 
 
typedef enum { 
    MIN_ACTION=300, 
    TAKE=MIN_ACTION, DROP, OPEN, CLOSE, ON, OFF, WAVE, CALM, GO, 
    RELAX, POUR, EAT, DRINK, RUB, TOSS, WAKE, FEED, FILL, BREAK, BLAST, 
    KILL, SAY, READ, FEEFIE, BRIEF, FIND, INVENTORY, SCORE, 
#ifdef SAVE_AND_RESTORE 
    SAVE, RESTORE, 
#endif /* SAVE_AND_RESTORE */ 
    QUIT, 
    MAX_ACTION=QUIT 
} ActionWord; 
 
typedef enum { 
    MIN_MESSAGE=400, 
    ABRA=MIN_MESSAGE, HELP, TREES, DIG, LOST, MIST, FUCK, STOP, INFO, SWIM, 
    MAX_MESSAGE=SWIM 
} MessageWord; 
 
WordClass word_class(int word) 
{ 
    if (word == NOTHING) { 
        return WordClass_None; 
    } else if (MIN_MOTION <= word && word <= MAX_MOTION) { 
        return WordClass_Motion; 
    } else if (MIN_OBJ <= word && word <= MAX_OBJ) { 
        return WordClass_Object; 
    } else if (MIN_ACTION <= word && word <= MAX_ACTION) { 
        return WordClass_Action; 
    } else if (MIN_MESSAGE <= word && word <= MAX_MESSAGE) { 
        return WordClass_Message; 
    } else { 
        assert(false); 
        return WordClass_None; 
    } 
} 
 
const char ok[] = GET_CSTR( CSTR_OK );  /* Woods' Fortran version didn't include the period, by the way. */ 
const char pitch_dark_msg[] = GET_CSTR( CSTR_IT_IS_NOW_PITCH_DARK_IF_YOU_PROCEED_YOU1 ); 
 
 
/*========== Locations. =================================================== 
 * This section corresponds to sections 18--62 in Knuth. 
 */ 
 
typedef enum { 
    R_INHAND = -1, R_LIMBO = 0, 
    R_ROAD, R_HILL, R_HOUSE, R_VALLEY, R_FOREST, R_FOREST2, R_SLIT, R_OUTSIDE, 
    R_INSIDE, MIN_IN_CAVE = R_INSIDE, 
    R_COBBLES, R_DEBRIS, R_AWK, R_BIRD, R_SPIT, 
    R_EMIST, MIN_LOWER_LOC = R_EMIST, 
    R_NUGGET, R_EFISS, R_WFISS, R_WMIST, 
    R_LIKE1, R_LIKE2, R_LIKE3, R_LIKE4, R_LIKE5, R_LIKE6, R_LIKE7, 
    R_LIKE8, R_LIKE9, R_LIKE10, R_LIKE11, R_LIKE12, R_LIKE13, R_LIKE14, 
    R_BRINK, R_ELONG, R_WLONG, 
    R_DIFF0, R_DIFF1, R_DIFF2, R_DIFF3, R_DIFF4, R_DIFF5, 
    R_DIFF6, R_DIFF7, R_DIFF8, R_DIFF9, R_DIFF10, 
    R_PONY, R_CROSS, R_HMK, R_WEST, R_SOUTH, R_NS, R_Y2, R_JUMBLE, R_WINDOE, 
    R_DIRTY, R_CLEAN, R_WET, R_DUSTY, R_COMPLEX, 
    R_SHELL, R_ARCHED, R_RAGGED, R_SAC, R_ANTE, R_WITT, 
    R_BEDQUILT, R_SWISS, R_SOFT, 
    R_E2PIT, R_W2PIT, R_EPIT, R_WPIT, 
    R_NARROW, R_GIANT, R_BLOCK, R_IMMENSE, 
    R_FALLS, R_INCLINE, R_ABOVEP, R_SJUNC, 
    R_TITE, R_LOW, R_CRAWL, R_WINDOW, 
    R_ORIENTAL, R_MISTY, R_ALCOVE, R_PLOVER, R_DARK, 
    R_SLAB, R_ABOVER, R_MIRROR, R_RES, 
    R_SCAN1, R_SCAN2, R_SCAN3, R_SECRET, 
    R_WIDE, R_TIGHT, R_TALL, R_BOULDERS, 
    R_SLOPING, R_SWSIDE, 
    R_DEAD0, R_DEAD1, R_PIRATES_NEST, R_DEAD3, R_DEAD4, R_DEAD5, 
    R_DEAD6, R_DEAD7, R_DEAD8, R_DEAD9, R_DEAD10, R_DEAD11, 
    R_NESIDE, R_CORR, R_FORK, R_WARM, R_VIEW, R_CHAMBER, 
    R_LIME, R_FBARR, R_BARR, 
    R_NEEND, R_SWEND, 
 
    R_NECK, R_LOSE, R_CLIMB, R_CHECK, 
    R_THRU, R_DUCK, R_UPNOUT, 
    R_DIDIT, MAX_LOC = R_DIDIT, 
    R_PPASS, R_PDROP, 
    R_TROLL, 
    FIRST_REMARK 
} Location; 
 
typedef struct { 
    MotionWord mot; 
    int cond; 
    Location dest; 
} Instruction; 
 
enum flagsBits { 
    F_CAVE_HINT  = 0x008, 
    F_BIRD_HINT  = 0x010, 
    F_SNAKE_HINT = 0x020, 
    F_TWIST_HINT = 0x040, 
    F_DARK_HINT  = 0x080, 
    F_WITT_HINT  = 0x100 
}; 
 
Instruction travels[733]; 
Instruction *start[MAX_LOC+2]; 
struct Place { 
    const char *long_desc; 
    const char *short_desc; 
    unsigned int flags; 
    ObjectWord objects;  /* first object at this location, or NOTHING */ 
    int visits; 
}; 
struct Place places[MAX_LOC+1]; 
 
void make_loc(Instruction *q, Location x, const char *l, const char *s, unsigned int f) 
{ 
    static int last_x = 0; 
    assert(x == last_x + 1); 
    last_x = x; 
 
    places[x].long_desc = l; 
    places[x].short_desc = s; 
    places[x].flags = f; 
    places[x].objects = NOTHING; 
    places[x].visits = 0; 
    start[x] = q; 
} 
 
void make_inst(Instruction *q, MotionWord m, int c, Location d) 
{ 
    assert(&travels[0] <= q && q < &travels[733]); 
    assert(m==0 || (MIN_MOTION <= m && m <= MAX_MOTION)); 
    q->mot = m; 
    q->cond = c; 
    q->dest = d; 
} 
 
#define make_ins(m, d) make_inst(q++, m, 0, d) 
#define make_cond_ins(m, c, d) make_inst(q++, m, c, d) 
#define ditto(m) make_inst(q, m, q[-1].cond, q[-1].dest); ++q; 
#define only_if_toting(t) (100 + (t-MIN_OBJ)) 
#define only_if_here(t) (200 + (t-MIN_OBJ)) 
#define unless_prop(t, p) (300 + (t-MIN_OBJ) + 100*p) 
 
#define remark(n) (FIRST_REMARK + n) 
 
void build_travel_table(void) 
{ 
    Instruction *q = travels; 
    static const char all_alike[] = GET_CSTR( CSTR_YOU_ARE_IN_MAZE_OF_TWISTY_LITTLE2 ); 
    static const char dead_end[] = GET_CSTR( CSTR_DEAD_END ); 
    make_loc(q, R_ROAD, 
             GET_CSTR( CSTR_YOU_ARE_STANDING_AT_THE_END_OF_ROAD1 ) SOFT_NL 
             GET_CSTR( CSTR_AROUND_YOU_IS_FOREST_SMALL_STREAM1 ) SOFT_NL 
             GET_CSTR( CSTR_DOWN_GULLY ), 
             GET_CSTR( CSTR_YOU_RE_AT_END_OF_ROAD_AGAIN1 ), 0); 
    make_ins(W, R_HILL); ditto(U); ditto(ROAD); 
    make_ins(E, R_HOUSE); ditto(IN); ditto(HOUSE); ditto(ENTER); 
    make_ins(S, R_VALLEY); ditto(D); ditto(GULLY); ditto(STREAM); ditto(DOWNSTREAM); 
    make_ins(N, R_FOREST); ditto(FOREST); 
    make_ins(DEPRESSION, R_OUTSIDE); 
    make_loc(q, R_HILL, 
             GET_CSTR( CSTR_YOU_HAVE_WALKED_UP_HILL_STILL_IN_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_DOWN_THE_OTHER_SIDE_OF_THE_HILL_THERE_IS ), 
             GET_CSTR( CSTR_YOU_RE_AT_HILL_IN_ROAD1 ), 0); 
    make_ins(ROAD, R_ROAD); ditto(HOUSE); ditto(FORWARD); ditto(E); ditto(D); 
    make_ins(FOREST, R_FOREST); ditto(N); ditto(S); 
    make_loc(q, R_HOUSE, 
             GET_CSTR( CSTR_YOU_ARE_INSIDE_BUILDING_WELL_HOUSE_FOR1 ), 
             GET_CSTR( CSTR_YOU_RE_INSIDE_BUILDING1 ),  0); 
    make_ins(ENTER, R_ROAD); ditto(OUT); ditto(OUTDOORS); ditto(W); 
    make_ins(XYZZY, R_DEBRIS); 
    make_ins(PLUGH, R_Y2); 
    make_ins(DOWNSTREAM, remark(17)); ditto(STREAM); 
    make_loc(q, R_VALLEY, 
             GET_CSTR( CSTR_YOU_ARE_IN_VALLEY_IN_THE_FOREST_BESIDE1 ) SOFT_NL 
             GET_CSTR( CSTR_ROCKY_BED ), 
             GET_CSTR( CSTR_YOU_RE_IN_VALLEY ), 0); 
    make_ins(UPSTREAM, R_ROAD); ditto(HOUSE); ditto(N); 
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(U); 
    make_ins(DOWNSTREAM, R_SLIT); ditto(S); ditto(D); 
    make_ins(DEPRESSION, R_OUTSIDE); 
    make_loc(q, R_FOREST, 
             GET_CSTR( CSTR_YOU_ARE_IN_OPEN_FOREST_WITH_DEEP_VALLEY1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_FOREST ), 0); 
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(D); 
    make_cond_ins(FOREST, 50, R_FOREST); ditto(FORWARD); ditto(N); 
    make_ins(FOREST, R_FOREST2); 
    make_ins(W, R_FOREST); ditto(S); 
    make_loc(q, R_FOREST2, 
             GET_CSTR( CSTR_YOU_ARE_IN_OPEN_FOREST_NEAR_BOTH_VALLEY1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_FOREST ), 0); 
    make_ins(ROAD, R_ROAD); ditto(N); 
    make_ins(VALLEY, R_VALLEY); ditto(E); ditto(W); ditto(D); 
    make_ins(FOREST, R_FOREST); ditto(S); 
    make_loc(q, R_SLIT, 
             GET_CSTR( CSTR_AT_YOUR_FEET_ALL_THE_WATER_OF_THE_STREAM1 ) SOFT_NL 
             GET_CSTR( CSTR_IN_THE_ROCK_DOWNSTREAM_THE_STREAMBED1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_SLIT_IN_STREAMBED1 ), 0); 
    make_ins(HOUSE, R_ROAD); 
    make_ins(UPSTREAM, R_VALLEY); ditto(N); 
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); 
    make_ins(DOWNSTREAM, R_OUTSIDE); ditto(ROCK); ditto(BED); ditto(S); 
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D); 
    make_loc(q, R_OUTSIDE, 
             GET_CSTR( CSTR_YOU_ARE_IN_FOOT_DEPRESSION_FLOORED1 ) SOFT_NL 
             GET_CSTR( CSTR_DIRT_IS_STRONG_STEEL_GRATE_MOUNTED_IN1 ) SOFT_NL 
             GET_CSTR( CSTR_LEADS_INTO_THE_DEPRESSION1 ), 
             GET_CSTR( CSTR_YOU_RE_OUTSIDE_GRATE1 ), F_CAVE_HINT); 
    make_ins(FOREST, R_FOREST); ditto(E); ditto(W); ditto(S); 
    make_ins(HOUSE, R_ROAD); 
    make_ins(UPSTREAM, R_SLIT); ditto(GULLY); ditto(N); 
    make_cond_ins(ENTER, unless_prop(GRATE, 0), R_INSIDE); ditto(IN); ditto(D); 
    make_ins(ENTER, remark(1)); 
    make_loc(q, R_INSIDE, 
             GET_CSTR( CSTR_YOU_ARE_IN_SMALL_CHAMBER_BENEATH_STEEL1 ) SOFT_NL 
             GET_CSTR( CSTR_LOW_CRAWL_OVER_COBBLES_LEADS_INWARD_TO1 ), 
             GET_CSTR( CSTR_YOU_RE_BELOW_THE_GRATE1 ), 0); 
    make_cond_ins(OUT, unless_prop(GRATE, 0), R_OUTSIDE); ditto(U); 
    make_ins(OUT, remark(1)); 
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(IN); ditto(W); 
    make_ins(PIT, R_SPIT); 
    make_ins(DEBRIS, R_DEBRIS); 
    make_loc(q, R_COBBLES, 
             GET_CSTR( CSTR_YOU_ARE_CRAWLING_OVER_COBBLES_IN_LOW1 ) SOFT_NL 
             GET_CSTR( CSTR_AT_THE_EAST_END_OF_THE_PASSAGE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_COBBLE_CRAWL1 ), 0); 
    make_ins(OUT, R_INSIDE); ditto(SURFACE); ditto(E); 
    make_ins(IN, R_DEBRIS); ditto(DARK); ditto(W); ditto(DEBRIS); 
    make_ins(PIT, R_SPIT); 
    make_loc(q, R_DEBRIS, 
             GET_CSTR( CSTR_YOU_ARE_IN_DEBRIS_ROOM_FILLED_WITH1 ) SOFT_NL 
             GET_CSTR( CSTR_LOW_WIDE_PASSAGE_WITH_COBBLES_BECOMES1 ) SOFT_NL 
             GET_CSTR( CSTR_HERE_BUT_AN_AWKWARD_CANYON_LEADS1 ) SOFT_NL 
             GET_CSTR( CSTR_SAYS_MAGIC_WORD_XYZZY1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_DEBRIS_ROOM1 ), 0); 
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE); 
    make_ins(ENTRANCE, R_INSIDE); 
    make_ins(CRAWL, R_COBBLES); ditto(COBBLES); ditto(PASSAGE); ditto(LOW); ditto(E); 
    make_ins(CANYON, R_AWK); ditto(IN); ditto(U); ditto(W); 
    make_ins(XYZZY, R_HOUSE); 
    make_ins(PIT, R_SPIT); 
    make_loc(q, R_AWK, 
             GET_CSTR( CSTR_YOU_ARE_IN_AN_AWKWARD_SLOPING_EAST_WEST1 ), 
             NULL, 0); 
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE); 
    make_ins(ENTRANCE, R_INSIDE); 
    make_ins(D, R_DEBRIS); ditto(E); ditto(DEBRIS); 
    make_ins(IN, R_BIRD); ditto(U); ditto(W); 
    make_ins(PIT, R_SPIT); 
    make_loc(q, R_BIRD, 
             GET_CSTR( CSTR_YOU_ARE_IN_SPLENDID_CHAMBER_THIRTY1 ) SOFT_NL 
             GET_CSTR( CSTR_RIVERS_OF_ORANGE_STONE_AN_AWKWARD1 ) SOFT_NL 
             GET_CSTR( CSTR_FROM_EAST_AND_WEST_SIDES_OF_THE_CHAMBER1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_BIRD_CHAMBER1 ), F_BIRD_HINT); 
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE); 
    make_ins(ENTRANCE, R_INSIDE); 
    make_ins(DEBRIS, R_DEBRIS); 
    make_ins(CANYON, R_AWK); ditto(E); 
    make_ins(PASSAGE, R_SPIT); ditto(PIT); ditto(W); 
    make_loc(q, R_SPIT, 
             GET_CSTR( CSTR_AT_YOUR_FEET_IS_SMALL_PIT_BREATHING1 ) SOFT_NL 
             GET_CSTR( CSTR_PASSAGE_ENDS_HERE_EXCEPT_FOR_SMALL1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_TOP_OF_SMALL_PIT1 ), 0); 
    make_cond_ins(DEPRESSION, unless_prop(GRATE, 0), R_OUTSIDE); 
    make_ins(ENTRANCE, R_INSIDE); 
    make_ins(DEBRIS, R_DEBRIS); 
    make_ins(PASSAGE, R_BIRD); ditto(E); 
    make_cond_ins(D, only_if_toting(GOLD), R_NECK); ditto(PIT); ditto(STEPS); 
    /* good thing you weren't loaded down with GOLD */ 
    make_ins(D, R_EMIST); 
    make_ins(CRACK, remark(14)); ditto(W); 
    make_loc(q, R_EMIST, 
             GET_CSTR( CSTR_YOU_ARE_AT_ONE_END_OF_VAST_HALL1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_WEST_THERE_ARE_OPENINGS_TO_EITHER1 ) SOFT_NL 
             GET_CSTR( CSTR_STAIRCASE_LEADS_DOWNWARD_THE_HALL_IS1 ) SOFT_NL 
             GET_CSTR( CSTR_SWAYING_TO_AND_FRO_ALMOST_AS_IF_ALIVE1 ) SOFT_NL 
             GET_CSTR( CSTR_STAIRCASE_THERE_IS_PASSAGE_AT_THE_TOP1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_HALL_OF_MISTS1 ), 0); 
    make_ins(LEFT, R_NUGGET); ditto(S); 
    make_ins(FORWARD, R_EFISS); ditto(HALL); ditto(W); 
    make_ins(STAIRS, R_HMK); ditto(D); ditto(N); 
    make_cond_ins(U, only_if_toting(GOLD), remark(15)); ditto(PIT); ditto(STEPS); ditto(DOME); ditto(PASSAGE); ditto(E); 
    make_ins(U, R_SPIT); 
    make_ins(Y2, R_JUMBLE); 
    make_loc(q, R_NUGGET, 
             GET_CSTR( CSTR_THIS_IS_LOW_ROOM_WITH_CRUDE_NOTE_ON_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_YOU_WON_GET_IT_UP_THE_STEPS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_NUGGET_OF_GOLD_ROOM1 ), 0); 
    make_ins(HALL, R_EMIST); ditto(OUT); ditto(N); 
    make_loc(q, R_EFISS, 
             GET_CSTR( CSTR_YOU_ARE_ON_THE_EAST_BANK_OF_FISSURE1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_MIST_IS_QUITE_THICK_HERE_AND_THE1 ), 
             GET_CSTR( CSTR_YOU_RE_ON_EAST_BANK_OF_FISSURE1 ), 0); 
    make_ins(HALL, R_EMIST); ditto(E); 
    make_cond_ins(JUMP, unless_prop(FISSURE, 0), remark(2)); 
    make_cond_ins(FORWARD, unless_prop(FISSURE, 1), R_LOSE); 
    make_cond_ins(OVER, unless_prop(FISSURE, 1), remark(3)); ditto(ACROSS); ditto(W); ditto(CROSS); 
    make_ins(OVER, R_WFISS); 
    make_loc(q, R_WFISS, 
             GET_CSTR( CSTR_YOU_ARE_ON_THE_WEST_SIDE_OF_THE_FISSURE1 ), 
             NULL, 0); 
    make_cond_ins(JUMP, unless_prop(FISSURE, 0), remark(2)); 
    make_cond_ins(FORWARD, unless_prop(FISSURE, 1), R_LOSE); 
    make_cond_ins(OVER, unless_prop(FISSURE, 1), remark(3)); ditto(ACROSS); ditto(E); ditto(CROSS); 
    make_ins(OVER, R_EFISS); 
    make_ins(W, R_WMIST); 
    make_ins(N, R_THRU); 
 
    make_loc(q, R_WMIST, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_WEST_END_OF_THE_HALL_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_CONTINUES_WEST_AND_ANOTHER_GOES_NORTH1 ) SOFT_NL 
             GET_CSTR( CSTR_PASSAGE_FEET_OFF_THE_FLOOR1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_WEST_END_OF_HALL_OF_MISTS1 ), 0); 
    make_ins(S, R_LIKE1); ditto(U); ditto(PASSAGE); ditto(CLIMB); 
    make_ins(E, R_WFISS); 
    make_ins(W, R_ELONG); ditto(CRAWL); 
    make_ins(N, R_DUCK); 
 
    make_loc(q, R_LIKE1, all_alike, NULL, F_TWIST_HINT); 
    make_ins(U, R_WMIST); 
    make_ins(N, R_LIKE1); 
    make_ins(E, R_LIKE2); 
    make_ins(S, R_LIKE4); 
    make_ins(W, R_LIKE11); 
    make_loc(q, R_LIKE2, all_alike, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE1); 
    make_ins(S, R_LIKE3); 
    make_ins(E, R_LIKE4); 
    make_loc(q, R_LIKE3, all_alike, NULL, F_TWIST_HINT); 
    make_ins(E, R_LIKE2); 
    make_ins(D, R_DEAD5); 
    make_ins(S, R_LIKE6); 
    make_ins(N, R_DEAD9); 
    make_loc(q, R_LIKE4, all_alike, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE1); 
    make_ins(N, R_LIKE2); 
    make_ins(E, R_DEAD3); 
    make_ins(S, R_DEAD4); 
    make_ins(U, R_LIKE14); ditto(D); 
    make_loc(q, R_LIKE5, all_alike, NULL, F_TWIST_HINT); 
    make_ins(E, R_LIKE6); 
    make_ins(W, R_LIKE7); 
    make_loc(q, R_LIKE6, all_alike, NULL, F_TWIST_HINT); 
    make_ins(E, R_LIKE3); 
    make_ins(W, R_LIKE5); 
    make_ins(D, R_LIKE7); 
    make_ins(S, R_LIKE8); 
    make_loc(q, R_LIKE7, all_alike, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE5); 
    make_ins(U, R_LIKE6); 
    make_ins(E, R_LIKE8); 
    make_ins(S, R_LIKE9); 
    make_loc(q, R_LIKE8, all_alike, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE6); 
    make_ins(E, R_LIKE7); 
    make_ins(S, R_LIKE8); 
    make_ins(U, R_LIKE9); 
    make_ins(N, R_LIKE10); 
    make_ins(D, R_DEAD11); 
    make_loc(q, R_LIKE9, all_alike, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE7); 
    make_ins(N, R_LIKE8); 
    make_ins(S, R_DEAD6); 
    make_loc(q, R_LIKE10, all_alike, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE8); 
    make_ins(N, R_LIKE10); 
    make_ins(D, R_DEAD7); 
    make_ins(E, R_BRINK); 
    make_loc(q, R_LIKE11, all_alike, NULL, F_TWIST_HINT); 
    make_ins(N, R_LIKE1); 
    make_ins(W, R_LIKE11); ditto(S); 
    make_ins(E, R_DEAD1); 
    make_loc(q, R_LIKE12, all_alike, NULL, F_TWIST_HINT); 
    make_ins(S, R_BRINK); 
    make_ins(E, R_LIKE13); 
    make_ins(W, R_DEAD10); 
    make_loc(q, R_LIKE13 , all_alike, NULL, F_TWIST_HINT); 
    make_ins(N, R_BRINK); 
    make_ins(W, R_LIKE12); 
    make_ins(NW, R_PIRATES_NEST);  /* NW: a dirty trick! */ 
    make_loc(q, R_LIKE14, all_alike, NULL, F_TWIST_HINT); 
    make_ins(U, R_LIKE4); ditto(D); 
    make_loc(q, R_BRINK, 
             GET_CSTR( CSTR_YOU_ARE_ON_THE_BRINK_OF_THIRTY_FOOT_PIT1 ) SOFT_NL 
             GET_CSTR( CSTR_DOWN_ONE_WALL_YOU_COULD_CLIMB_DOWN_HERE1 ) SOFT_NL 
             GET_CSTR( CSTR_UP_THE_MAZE_CONTINUES_AT_THIS_LEVEL1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_BRINK_OF_PIT1 ), 0); 
    make_ins(D, R_BIRD); ditto(CLIMB); 
    make_ins(W, R_LIKE10); 
    make_ins(S, R_DEAD8); 
    make_ins(N, R_LIKE12); 
    make_ins(E, R_LIKE13); 
    make_loc(q, R_ELONG, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_EAST_END_OF_VERY_LONG_HALL1 ) SOFT_NL 
             GET_CSTR( CSTR_CHAMBERS_TO_THE_EAST_LOW_WIDE_CRAWL1 ) SOFT_NL 
             GET_CSTR( CSTR_ROUND_TWO_FOOT_HOLE_SLANTS_DOWN1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_EAST_END_OF_LONG_HALL1 ), 0); 
    make_ins(E, R_WMIST); ditto(U); ditto(CRAWL); 
    make_ins(W, R_WLONG); 
    make_ins(N, R_CROSS); ditto(D); ditto(HOLE); 
    make_loc(q, R_WLONG, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_WEST_END_OF_VERY_LONG1 ) SOFT_NL 
             GET_CSTR( CSTR_JOINS_UP_WITH_NARROW_NORTH_SOUTH1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_WEST_END_OF_LONG_HALL1 ), 0); 
    make_ins(E, R_ELONG); 
    make_ins(N, R_CROSS); 
    make_cond_ins(S, 100, R_DIFF0);  /* 100: Dwarves Not Permitted. */ 
 
#define twist(name,n,s,e,w,ne,se,nw,sw,u,d,m)  \ 
        make_loc(q, name, m, NULL, 0); \ 
        make_ins(N,n); make_ins(S,s); make_ins(E,e); make_ins(W,w); \ 
        make_ins(NE,ne); make_ins(SE,se); make_ins(NW,nw); make_ins(SW,sw); \ 
        make_ins(U,u); make_ins(D,d); 
 
    twist(R_DIFF0,R_DIFF9,R_DIFF1,R_DIFF7,R_DIFF8,R_DIFF3,R_DIFF4,R_DIFF6,R_DIFF2,R_DIFF5,R_WLONG, 
        GET_CSTR( CSTR_YOU_ARE_IN_MAZE_OF_TWISTY_LITTLE3 )); 
    twist(R_DIFF1,R_DIFF8,R_DIFF9,R_DIFF10,R_DIFF0,R_DIFF5,R_DIFF2,R_DIFF3,R_DIFF4,R_DIFF6,R_DIFF7, 
        GET_CSTR( CSTR_YOU_ARE_IN_MAZE_OF_TWISTING_LITTLE1 )); 
    twist(R_DIFF2,R_DIFF3,R_DIFF4,R_DIFF8,R_DIFF5,R_DIFF7,R_DIFF10,R_DIFF0,R_DIFF6,R_DIFF1,R_DIFF9, 
        GET_CSTR( CSTR_YOU_ARE_IN_LITTLE_MAZE_OF_TWISTY1 )); 
    twist(R_DIFF3,R_DIFF7,R_DIFF10,R_DIFF6,R_DIFF2,R_DIFF4,R_DIFF9,R_DIFF8,R_DIFF5,R_DIFF0,R_DIFF1, 
        GET_CSTR( CSTR_YOU_ARE_IN_TWISTING_MAZE_OF_LITTLE1 )); 
    twist(R_DIFF4,R_DIFF1,R_DIFF7,R_DIFF5,R_DIFF9,R_DIFF0,R_DIFF3,R_DIFF2,R_DIFF10,R_DIFF8,R_DIFF6, 
        GET_CSTR( CSTR_YOU_ARE_IN_TWISTING_LITTLE_MAZE_OF1 )); 
    twist(R_DIFF5,R_DIFF0,R_DIFF3,R_DIFF4,R_DIFF6,R_DIFF8,R_DIFF1,R_DIFF9,R_DIFF7,R_DIFF10,R_DIFF2, 
        GET_CSTR( CSTR_YOU_ARE_IN_TWISTY_LITTLE_MAZE_OF1 )); 
    twist(R_DIFF6,R_DIFF10,R_DIFF5,R_DIFF0,R_DIFF1,R_DIFF9,R_DIFF8,R_DIFF7,R_DIFF3,R_DIFF2,R_DIFF4, 
        GET_CSTR( CSTR_YOU_ARE_IN_TWISTY_MAZE_OF_LITTLE1 )); 
    twist(R_DIFF7,R_DIFF6,R_DIFF2,R_DIFF9,R_DIFF10,R_DIFF1,R_DIFF0,R_DIFF5,R_DIFF8,R_DIFF4,R_DIFF3, 
        GET_CSTR( CSTR_YOU_ARE_IN_LITTLE_TWISTY_MAZE_OF1 )); 
    twist(R_DIFF8,R_DIFF5,R_DIFF6,R_DIFF1,R_DIFF4,R_DIFF2,R_DIFF7,R_DIFF10,R_DIFF9,R_DIFF3,R_DIFF0, 
        GET_CSTR( CSTR_YOU_ARE_IN_MAZE_OF_LITTLE_TWISTING1 )); 
    twist(R_DIFF9,R_DIFF4,R_DIFF8,R_DIFF2,R_DIFF3,R_DIFF10,R_DIFF6,R_DIFF1,R_DIFF0,R_DIFF7,R_DIFF5, 
        GET_CSTR( CSTR_YOU_ARE_IN_MAZE_OF_LITTLE_TWISTY1 )); 
    twist(R_DIFF10,R_DIFF2,R_PONY,R_DIFF3,R_DIFF7,R_DIFF6,R_DIFF5,R_DIFF4,R_DIFF1,R_DIFF9,R_DIFF8, 
        GET_CSTR( CSTR_YOU_ARE_IN_LITTLE_MAZE_OF_TWISTING1 )); 
 
#undef twist 
 
    make_loc(q, R_PONY, dead_end, NULL, 0); 
    make_ins(N, R_DIFF10); ditto(OUT); 
    make_loc(q, R_CROSS, 
             GET_CSTR( CSTR_YOU_ARE_AT_CROSSOVER_OF_HIGH_PASSAGE1 ), 
             NULL, 0); 
    make_ins(W, R_ELONG); 
    make_ins(N, R_DEAD0); 
    make_ins(E, R_WEST); 
    make_ins(S, R_WLONG); 
    make_loc(q, R_HMK, 
        GET_CSTR( CSTR_YOU_ARE_IN_THE_HALL_OF_THE_MOUNTAIN_KING1 ) SOFT_NL 
        GET_CSTR( CSTR_DIRECTIONS ), 
        GET_CSTR( CSTR_YOU_RE_IN_HALL_OF_MT_KING1 ), F_SNAKE_HINT); 
    make_ins(STAIRS, R_EMIST); ditto(U); ditto(E); 
    /* I suppose our adventurer must be walking on the ceiling! */ 
    make_cond_ins(N, unless_prop(SNAKE, 0), R_NS); ditto(LEFT); 
    make_cond_ins(S, unless_prop(SNAKE, 0), R_SOUTH); ditto(RIGHT); 
    make_cond_ins(W, unless_prop(SNAKE, 0), R_WEST); ditto(FORWARD); 
    make_ins(N, remark(16)); 
    make_cond_ins(SW, 35, R_SECRET); 
    make_cond_ins(SW, only_if_here(SNAKE), remark(16)); 
    make_ins(SECRET, R_SECRET); 
    make_loc(q, R_WEST, 
        GET_CSTR( CSTR_YOU_ARE_IN_THE_WEST_SIDE_CHAMBER_OF_THE1 ) SOFT_NL 
        GET_CSTR( CSTR_PASSAGE_CONTINUES_WEST_AND_UP_HERE1 ), 
        GET_CSTR( CSTR_YOU_RE_IN_WEST_SIDE_CHAMBER1 ), 0); 
    make_ins(HALL, R_HMK); ditto(OUT); ditto(E); 
    make_ins(W, R_CROSS); ditto(U); 
    make_loc(q, R_SOUTH, 
        GET_CSTR( CSTR_YOU_ARE_IN_THE_SOUTH_SIDE_CHAMBER1 ), 
        NULL, 0); 
    make_ins(HALL, R_HMK); ditto(OUT); ditto(N); 
    make_loc(q, R_NS, 
             GET_CSTR( CSTR_YOU_ARE_IN_LOW_PASSAGE_AT_HOLE_IN_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_DOWN_TO_AN_PASSAGE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_PASSAGE1 ), 0); 
    make_ins(HALL, R_HMK); ditto(OUT); ditto(S); 
    make_ins(N, R_Y2); ditto(Y2); 
    make_ins(D, R_DIRTY); ditto(HOLE); 
    make_loc(q, R_Y2, 
             GET_CSTR( CSTR_YOU_ARE_IN_LARGE_ROOM_WITH_PASSAGE_TO1 ) SOFT_NL 
             GET_CSTR( CSTR_WEST_AND_WALL_OF_BROKEN_ROCK_TO_THE_EAST1 ) SOFT_NL 
             GET_CSTR( CSTR_ROCK_IN_THE_ROOM_CENTER1 ), 
             GET_CSTR( CSTR_YOU_RE_AT ), 0); 
    make_ins(PLUGH, R_HOUSE); 
    make_ins(S, R_NS); 
    make_ins(E, R_JUMBLE); ditto(WALL); ditto(BROKEN); 
    make_ins(W, R_WINDOE); 
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP); 
    make_ins(PLOVER, R_PLOVER); 
    make_loc(q, R_JUMBLE, 
             GET_CSTR( CSTR_YOU_ARE_IN_JUMBLE_OF_ROCK_WITH_CRACKS1 ), 
             NULL, 0); 
    make_ins(D, R_Y2); ditto(Y2); 
    make_ins(U, R_EMIST); 
    make_loc(q, R_WINDOE, 
             GET_CSTR( CSTR_YOU_RE_AT_LOW_WINDOW_OVERLOOKING_HUGE2 ) SOFT_NL 
             GET_CSTR( CSTR_SIGHT_FLOOR_IS_INDISTINCTLY_VISIBLE2 ) SOFT_NL 
             GET_CSTR( CSTR_WHITE_MIST_COVER_THE_FLOOR_OF_THE_PIT2 ) SOFT_NL 
             GET_CSTR( CSTR_MARKS_IN_THE_DUST_AROUND_THE_WINDOW2 ) SOFT_NL 
             GET_CSTR( CSTR_SOMEONE_HAS_BEEN_HERE_RECENTLY2 ) SOFT_NL 
             GET_CSTR( CSTR_FEET_AWAY_THERE_IS_SIMILAR_WINDOW2 ) SOFT_NL 
             GET_CSTR( CSTR_SHADOWY_FIGURE_CAN_BE_SEEN_THERE2 ), 
             GET_CSTR( CSTR_YOU_RE_AT_WINDOW_ON_PIT2 ), 0); 
    make_ins(E, R_Y2); ditto(Y2); 
    make_ins(JUMP, R_NECK); 
    make_loc(q, R_DIRTY, 
             GET_CSTR( CSTR_YOU_ARE_IN_DIRTY_BROKEN_PASSAGE_TO_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_WEST_IS_LARGE_PASSAGE_ABOVE_YOU_IS_HOLE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_DIRTY_PASSAGE1 ), 0); 
    make_ins(E, R_CLEAN); ditto(CRAWL); 
    make_ins(U, R_NS); ditto(HOLE); 
    make_ins(W, R_DUSTY); 
    make_ins(BEDQUILT, R_BEDQUILT); 
    make_loc(q, R_CLEAN, 
             GET_CSTR( CSTR_YOU_ARE_ON_THE_BRINK_OF_SMALL_CLEAN1 ) SOFT_NL 
             GET_CSTR( CSTR_WEST1 ), 
             GET_CSTR( CSTR_YOU_RE_BY_CLEAN_PIT1 ), 0); 
    make_ins(W, R_DIRTY); ditto(CRAWL); 
    make_ins(D, R_WET); ditto(PIT); ditto(CLIMB); 
    make_loc(q, R_WET, 
             GET_CSTR( CSTR_YOU_ARE_IN_THE_BOTTOM_OF_SMALL_PIT_WITH1 ) SOFT_NL 
             GET_CSTR( CSTR_ENTERS_AND_EXITS_THROUGH_TINY_SLITS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_PIT_BY_STREAM1 ), 0); 
    make_ins(CLIMB, R_CLEAN); ditto(U); ditto(OUT); 
    make_ins(SLIT, remark(0)); ditto(STREAM); ditto(D); ditto(UPSTREAM); ditto(DOWNSTREAM); 
    make_loc(q, R_DUSTY, 
             GET_CSTR( CSTR_YOU_ARE_IN_LARGE_ROOM_FULL_OF_DUSTY1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_FLOOR_THERE_ARE_CRACKS_EVERYWHERE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_DUSTY_ROCK_ROOM1 ), 0); 
    make_ins(E, R_DIRTY); ditto(PASSAGE); 
    make_ins(D, R_COMPLEX); ditto(HOLE); ditto(FLOOR); 
    make_ins(BEDQUILT, R_BEDQUILT); 
    make_loc(q, R_COMPLEX, 
             GET_CSTR( CSTR_YOU_ARE_AT_COMPLEX_JUNCTION_LOW_HANDS1 ) SOFT_NL 
             GET_CSTR( CSTR_NORTH_JOINS_HIGHER_CRAWL_FROM_THE_EAST1 ) SOFT_NL 
             GET_CSTR( CSTR_GOING_WEST_THERE_IS_ALSO_LARGE_ROOM1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_COMPLEX_JUNCTION1 ), 0); 
    make_ins(U, R_DUSTY); ditto(CLIMB); ditto(ROOM); 
    make_ins(W, R_BEDQUILT); ditto(BEDQUILT); 
    make_ins(N, R_SHELL); ditto(SHELL); 
    make_ins(E, R_ANTE); 
    make_loc(q, R_SHELL, 
             GET_CSTR( CSTR_YOU_RE_IN_LARGE_ROOM_CARVED_OUT_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_AND_WALLS_ARE_LITTERED_WITH_BITS_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_SHALLOW_PASSAGE_PROCEEDS_DOWNWARD1 ) SOFT_NL 
             GET_CSTR( CSTR_LEADS_UP_LOW_HANDS_AND_KNEES_PASSAGE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_SHELL_ROOM1 ), 0); 
    make_ins(U, R_ARCHED); ditto(HALL); 
    make_ins(D, R_RAGGED); 
    make_cond_ins(S, only_if_toting(CLAM), remark(4)); 
    make_cond_ins(S, only_if_toting(OYSTER), remark(5)); 
    make_ins(S, R_COMPLEX); 
    make_loc(q, R_ARCHED, 
             GET_CSTR( CSTR_YOU_ARE_IN_AN_ARCHED_HALL_CORAL_PASSAGE1 ) SOFT_NL 
             GET_CSTR( CSTR_FROM_HERE_BUT_IS_NOW_BLOCKED_BY_DEBRIS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_ARCHED_HALL1 ), 0); 
    make_ins(D, R_SHELL); ditto(SHELL); ditto(OUT); 
    make_loc(q, R_RAGGED, 
             GET_CSTR( CSTR_YOU_ARE_IN_LONG_SLOPING_CORRIDOR_WITH1 ), 
             NULL, 0); 
    make_ins(U, R_SHELL); ditto(SHELL); 
    make_ins(D, R_SAC); 
    make_loc(q, R_SAC, 
             GET_CSTR( CSTR_YOU_ARE_IN_CUL_DE_SAC_ABOUT_EIGHT_FEET1 ), 
             NULL, 0); 
    make_ins(U, R_RAGGED); ditto(OUT); 
    make_ins(SHELL, R_SHELL); 
    make_loc(q, R_ANTE, 
             GET_CSTR( CSTR_YOU_ARE_IN_AN_ANTEROOM_LEADING_TO_LARGE1 ) SOFT_NL 
             GET_CSTR( CSTR_PASSAGES_GO_WEST_AND_UP_THE_REMNANTS_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_SIGN_IN_MIDAIR_HERE_SAYS_CAVE_UNDER1 ) SOFT_NL 
             GET_CSTR( CSTR_PROCEED_AT_OWN_RISK_WITT_CONSTRUCTION1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_ANTEROOM ), 0); 
    make_ins(U, R_COMPLEX); 
    make_ins(W, R_BEDQUILT); 
    make_ins(E, R_WITT); 
    make_loc(q, R_WITT, 
             GET_CSTR( CSTR_YOU_ARE_AT_WITT_END_PASSAGES_LEAD_OFF_IN1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_WITT_END1 ), F_WITT_HINT); 
    make_cond_ins(E, 95, remark(6)); ditto(N); ditto(S); 
    ditto(NE); ditto(SE); ditto(SW); ditto(NW); ditto(U); ditto(D); 
    make_ins(E, R_ANTE); 
    make_ins(W, remark(7)); 
 
    make_loc(q, R_BEDQUILT, 
             GET_CSTR( CSTR_YOU_ARE_IN_BEDQUILT_LONG_EAST_WEST1 ) SOFT_NL 
             GET_CSTR( CSTR_TO_EXPLORE_AT_RANDOM_SELECT_NORTH1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_BEDQUILT ), 0); 
    make_ins(E, R_COMPLEX); 
    make_ins(W, R_SWISS); 
    make_cond_ins(S, 80, remark(6)); 
    make_ins(SLAB, R_SLAB); 
    make_cond_ins(U, 80, remark(6)); 
    make_cond_ins(U, 50, R_ABOVEP); 
    make_ins(U, R_DUSTY); 
    make_cond_ins(N, 60, remark(6)); 
    make_cond_ins(N, 75, R_LOW); 
    make_ins(N, R_SJUNC); 
    make_cond_ins(D, 80, remark(6)); 
    make_ins(D, R_ANTE); 
 
    make_loc(q, R_SWISS, 
             GET_CSTR( CSTR_YOU_ARE_IN_ROOM_WHOSE_WALLS_RESEMBLE1 ) SOFT_NL 
             GET_CSTR( CSTR_GO_WEST_EAST_NE_AND_NW_PART_OF_THE_ROOM_IS1 ) SOFT_NL 
             GET_CSTR( CSTR_BEDROCK_BLOCK ), 
             GET_CSTR( CSTR_YOU_RE_IN_SWISS_CHEESE_ROOM1 ), 0); 
    make_ins(NE, R_BEDQUILT); 
    make_ins(W, R_E2PIT); 
    make_cond_ins(S, 80, remark(6)); 
    make_ins(CANYON, R_TALL); 
    make_ins(E, R_SOFT); 
    make_cond_ins(NW, 50, remark(6)); 
    make_ins(ORIENTAL, R_ORIENTAL); 
    make_loc(q, R_SOFT, 
             GET_CSTR( CSTR_YOU_ARE_IN_THE_SOFT_ROOM_THE_WALLS_ARE1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_FLOOR_WITH_THICK_PILE_CARPET_MOSS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_SOFT_ROOM ), 0); 
    make_ins(W, R_SWISS); ditto(OUT); 
    make_loc(q, R_E2PIT, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_EAST_END_OF_THE_TWOPIT1 ) SOFT_NL 
             GET_CSTR( CSTR_LITTERED_WITH_THIN_ROCK_SLABS_WHICH1 ) SOFT_NL 
             GET_CSTR( CSTR_THERE_IS_PATH_HERE_BYPASSING_THE_PITS1 ) SOFT_NL 
             GET_CSTR( CSTR_AND_WEST_THERE_ARE_HOLES_ALL_OVER_BUT1 ) SOFT_NL 
             GET_CSTR( CSTR_WALL_DIRECTLY_OVER_THE_WEST_PIT_WHERE1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_EAST_END_OF_TWOPIT_ROOM1 ), 0); 
    make_ins(E, R_SWISS); 
    make_ins(W, R_W2PIT); ditto(ACROSS); 
    make_ins(D, R_EPIT); ditto(PIT); 
    make_loc(q, R_W2PIT, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_WEST_END_OF_THE_TWOPIT1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_WALL_ABOVE_THE_PIT_AT_THIS_END_OF_THE1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_WEST_END_OF_TWOPIT_ROOM1 ), 0); 
    make_ins(E, R_E2PIT); ditto(ACROSS); 
    make_ins(W, R_SLAB); ditto(SLAB); 
    make_ins(D, R_WPIT); ditto(PIT); 
    make_ins(HOLE, remark(8)); 
    make_loc(q, R_EPIT, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_BOTTOM_OF_THE_EASTERN_PIT1 ) SOFT_NL 
             GET_CSTR( CSTR_SMALL_POOL_OF_OIL_IN_ONE_CORNER_OF_THE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_EAST_PIT ), 0); 
    make_ins(U, R_E2PIT); ditto(OUT); 
    make_loc(q, R_WPIT, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_BOTTOM_OF_THE_WESTERN_PIT1 ) SOFT_NL 
             GET_CSTR( CSTR_LARGE_HOLE_IN_THE_WALL_ABOUT_FEET_ABOVE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_WEST_PIT ), 0); 
    make_ins(U, R_W2PIT); ditto(OUT); 
    make_cond_ins(CLIMB, unless_prop(PLANT, 2), R_CHECK); 
    make_ins(CLIMB, R_CLIMB); 
    make_loc(q, R_NARROW, 
             GET_CSTR( CSTR_YOU_ARE_IN_LONG_NARROW_CORRIDOR1 ) SOFT_NL 
             GET_CSTR( CSTR_WEST_AT_THE_EASTERN_END_IS_HOLE_THROUGH1 ) SOFT_NL 
             GET_CSTR( CSTR_PROFUSION_OF_LEAVES ), 
             GET_CSTR( CSTR_YOU_RE_IN_NARROW_CORRIDOR1 ), 0); 
    make_ins(D, R_WPIT); ditto(CLIMB); ditto(E); 
    make_ins(JUMP, R_NECK); 
    make_ins(W, R_GIANT); ditto(GIANT); 
    make_loc(q, R_GIANT, 
             GET_CSTR( CSTR_YOU_ARE_IN_THE_GIANT_ROOM_THE_CEILING1 ) SOFT_NL 
             GET_CSTR( CSTR_LAMP_TO_SHOW_IT_CAVERNOUS_PASSAGES1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_WEST_WALL_IS_SCRAWLED_THE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_GIANT_ROOM1 ), 0); 
    make_ins(S, R_NARROW); 
    make_ins(E, R_BLOCK); 
    make_ins(N, R_IMMENSE); 
    make_loc(q, R_BLOCK, 
             GET_CSTR( CSTR_THE_PASSAGE_HERE_IS_BLOCKED_BY_RECENT1 ), 
             NULL, 0); 
    make_ins(S, R_GIANT); ditto(GIANT); ditto(OUT); 
    make_loc(q, R_IMMENSE, 
             GET_CSTR( CSTR_YOU_ARE_AT_ONE_END_OF_AN_IMMENSE_NORTH1 ), 
             NULL, 0); 
    make_ins(S, R_GIANT); ditto(GIANT); ditto(PASSAGE); 
    make_cond_ins(N, unless_prop(RUSTY_DOOR, 0), R_FALLS); ditto(ENTER); ditto(CAVERN); 
    make_ins(N, remark(9)); 
    make_loc(q, R_FALLS, 
             GET_CSTR( CSTR_YOU_ARE_IN_MAGNIFICENT_CAVERN_WITH1 ) SOFT_NL 
             GET_CSTR( CSTR_OVER_SPARKLING_WATERFALL_INTO1 ) SOFT_NL 
             GET_CSTR( CSTR_THROUGH_HOLE_IN_THE_FLOOR_PASSAGES1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_CAVERN_WITH_WATERFALL1 ), 0); 
    make_ins(S, R_IMMENSE); ditto(OUT); 
    make_ins(GIANT, R_GIANT); 
    make_ins(W, R_INCLINE); 
    make_loc(q, R_INCLINE, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_TOP_OF_STEEP_INCLINE1 ) SOFT_NL 
             GET_CSTR( CSTR_CLIMB_DOWN_HERE_BUT_YOU_WOULD_NOT_BE1 ) SOFT_NL 
             GET_CSTR( CSTR_PASSAGE_LEADING_BACK_TO_THE_NORTH1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_STEEP_INCLINE_ABOVE_LARGE1 ), 0); 
    make_ins(N, R_FALLS); ditto(CAVERN); ditto(PASSAGE); 
    make_ins(D, R_LOW); ditto(CLIMB); 
    make_loc(q, R_ABOVEP, 
             GET_CSTR( CSTR_YOU_ARE_IN_SECRET_CANYON_ABOVE_SIZABLE1 ), 
             NULL, 0); 
    make_ins(N, R_SJUNC); 
    make_ins(D, R_BEDQUILT); ditto(PASSAGE); 
    make_ins(S, R_TITE); 
    make_loc(q, R_SJUNC, 
             GET_CSTR( CSTR_YOU_ARE_IN_SECRET_CANYON_AT_JUNCTION_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_NORTH_SOUTH_AND_SE_THE_NORTH_ONE_IS_AS1 ) SOFT_NL 
             GET_CSTR( CSTR_COMBINED ), 
             GET_CSTR( CSTR_YOU_RE_AT_JUNCTION_OF_THREE_SECRET1 ), 0); 
    /* In Crowther's original, this was pretty much the edge of the cave. Going UP here 
     * would take you on a one-way trip back to the dusty rock room. Woods replaced 
     * that connection with a northerly passage to R_WINDOW. */ 
    make_ins(SE, R_BEDQUILT); 
    make_ins(S, R_ABOVEP); 
    make_ins(N, R_WINDOW); 
    make_loc(q, R_TITE, 
             GET_CSTR( CSTR_LARGE_STALACTITE_EXTENDS_FROM_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_BELOW_YOU_COULD_CLIMB_DOWN_IT_AND_JUMP1 ) SOFT_NL 
             GET_CSTR( CSTR_HAVING_DONE_SO_YOU_WOULD_BE_UNABLE_TO1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_TOP_OF_STALACTITE1 ), 0); 
    make_ins(N, R_ABOVEP); 
    make_cond_ins(D, 40, R_LIKE6); ditto(JUMP); ditto(CLIMB); 
    make_cond_ins(D, 50, R_LIKE9); 
    make_ins(D, R_LIKE4); 
    make_loc(q, R_LOW, 
             GET_CSTR( CSTR_YOU_ARE_IN_LARGE_LOW_ROOM_CRAWLS_LEAD1 ), 
             NULL, 0); 
    make_ins(BEDQUILT, R_BEDQUILT); 
    make_ins(SW, R_SLOPING); 
    make_ins(N, R_CRAWL); 
    make_ins(SE, R_ORIENTAL); ditto(ORIENTAL); 
    make_loc(q, R_CRAWL, 
             GET_CSTR( CSTR_DEAD_END_CRAWL ), 
             NULL, 0); 
    make_ins(S, R_LOW); ditto(CRAWL); ditto(OUT); 
    make_loc(q, R_WINDOW, 
             GET_CSTR( CSTR_YOU_RE_AT_LOW_WINDOW_OVERLOOKING_HUGE3 ) SOFT_NL 
             GET_CSTR( CSTR_SIGHT_FLOOR_IS_INDISTINCTLY_VISIBLE3 ) SOFT_NL 
             GET_CSTR( CSTR_WHITE_MIST_COVER_THE_FLOOR_OF_THE_PIT3 ) SOFT_NL 
             GET_CSTR( CSTR_MARKS_IN_THE_DUST_AROUND_THE_WINDOW3 ) SOFT_NL 
             GET_CSTR( CSTR_SOMEONE_HAS_BEEN_HERE_RECENTLY3 ) SOFT_NL 
             GET_CSTR( CSTR_FEET_AWAY_THERE_IS_SIMILAR_WINDOW3 ) SOFT_NL 
             GET_CSTR( CSTR_SHADOWY_FIGURE_CAN_BE_SEEN_THERE3 ), 
             GET_CSTR( CSTR_YOU_RE_AT_WINDOW_ON_PIT3 ), 0); 
    make_ins(W, R_SJUNC); 
    make_ins(JUMP, R_NECK); 
    make_loc(q, R_ORIENTAL, 
             GET_CSTR( CSTR_THIS_IS_THE_ORIENTAL_ROOM_ANCIENT1 ) SOFT_NL 
             GET_CSTR( CSTR_WALLS_GENTLY_SLOPING_PASSAGE_LEADS1 ) SOFT_NL 
             GET_CSTR( CSTR_PASSAGE_LEADS_SE_AND_HANDS_AND_KNEES1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_ORIENTAL_ROOM1 ), 0); 
    make_ins(SE, R_SWISS); 
    make_ins(W, R_LOW); ditto(CRAWL); 
    make_ins(U, R_MISTY); ditto(N); ditto(CAVERN); 
    make_loc(q, R_MISTY, 
             GET_CSTR( CSTR_YOU_ARE_FOLLOWING_WIDE_PATH_AROUND_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_FAR_BELOW_THROUGH_HEAVY_WHITE_MIST1 ) SOFT_NL 
             GET_CSTR( CSTR_HEARD_THE_MIST_RISES_UP_THROUGH1 ) SOFT_NL 
             GET_CSTR( CSTR_EXITS_TO_THE_SOUTH_AND_WEST1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_MISTY_CAVERN1 ), 0); 
    make_ins(S, R_ORIENTAL); ditto(ORIENTAL); 
    make_ins(W, R_ALCOVE); 
    make_loc(q, R_ALCOVE, 
             GET_CSTR( CSTR_YOU_ARE_IN_AN_ALCOVE_SMALL_NW_PATH_SEEMS1 ) SOFT_NL 
             GET_CSTR( CSTR_DISTANCE_AN_EXTREMELY_TIGHT_TUNNEL1 ) SOFT_NL 
             GET_CSTR( CSTR_TIGHT_SQUEEZE_AN_EERIE_LIGHT_CAN_BE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_ALCOVE ), F_DARK_HINT); 
    make_ins(NW, R_MISTY); ditto(CAVERN); 
    make_ins(E, R_PPASS); ditto(PASSAGE); 
    make_ins(E, R_PLOVER);  /* never performed, but seen by "BACK" */ 
    make_loc(q, R_PLOVER, 
             GET_CSTR( CSTR_YOU_RE_IN_SMALL_CHAMBER_LIT_BY_AN_EERIE1 ) SOFT_NL 
             GET_CSTR( CSTR_NARROW_TUNNEL_EXITS_TO_THE_WEST_DARK1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_PLOVER_ROOM1 ), F_DARK_HINT); 
    make_ins(W, R_PPASS); ditto(PASSAGE); ditto(OUT); 
    make_ins(W, R_ALCOVE);  /* never performed, but seen by "BACK" */ 
    make_cond_ins(PLOVER, only_if_toting(EMERALD), R_PDROP); 
    make_ins(PLOVER, R_Y2); 
    make_ins(NE, R_DARK); ditto(DARK); 
    make_loc(q, R_DARK, 
             GET_CSTR( CSTR_YOU_RE_IN_THE_DARK_ROOM_CORRIDOR1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_DARK_ROOM ), F_DARK_HINT); 
    make_ins(S, R_PLOVER); ditto(PLOVER); ditto(OUT); 
    make_loc(q, R_SLAB, 
             GET_CSTR( CSTR_YOU_ARE_IN_LARGE_LOW_CIRCULAR_CHAMBER1 ) SOFT_NL 
             GET_CSTR( CSTR_FALLEN_FROM_THE_CEILING_SLAB_ROOM1 ) SOFT_NL 
             GET_CSTR( CSTR_TO_THE_EAST_AND_WEST_BUT_THEY_ARE_NOW1 ) SOFT_NL 
             GET_CSTR( CSTR_SMALL_PASSAGES_GO_NORTH_AND_SOUTH_AND1 ) SOFT_NL 
             GET_CSTR( CSTR_EAST_AROUND_THE_BOULDERS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_SLAB_ROOM ), 0); 
    make_ins(S, R_W2PIT); 
    make_ins(U, R_ABOVER); ditto(CLIMB); 
    make_ins(N, R_BEDQUILT); 
    make_loc(q, R_ABOVER, 
             GET_CSTR( CSTR_YOU_ARE_IN_SECRET_CANYON_ABOVE_LARGE1 ), 
             NULL, 0); 
    make_ins(D, R_SLAB); ditto(SLAB); 
    make_cond_ins(S, unless_prop(DRAGON, 0), R_SCAN2); 
    make_ins(S, R_SCAN1); 
    make_ins(N, R_MIRROR); 
    make_ins(RESERVOIR, R_RES); 
    make_loc(q, R_MIRROR, 
             GET_CSTR( CSTR_YOU_ARE_IN_NORTH_SOUTH_CANYON_ABOUT1 ) SOFT_NL 
             GET_CSTR( CSTR_COVERED_BY_WHITE_MIST_SEEPING_IN_FROM1 ) SOFT_NL 
             GET_CSTR( CSTR_UPWARD_FOR_WELL_OVER_FEET_SUSPENDED1 ) SOFT_NL 
             GET_CSTR( CSTR_ABOVE_YOU_AN_ENORMOUS_TWO_SIDED_MIRROR1 ) SOFT_NL 
             GET_CSTR( CSTR_MIDWAY_BETWEEN_THE_CANYON_WALLS_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_FOR_THE_USE_OF_THE_DWARVES_WHO_AS_YOU1 ) SOFT_NL 
             GET_CSTR( CSTR_SMALL_WINDOW_CAN_BE_SEEN_IN_EITHER_WALL1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_MIRROR_CANYON1 ), 0); 
    make_ins(S, R_ABOVER); 
    make_ins(N, R_RES); ditto(RESERVOIR); 
    make_loc(q, R_RES, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_EDGE_OF_LARGE1 ) SOFT_NL 
             GET_CSTR( CSTR_OF_WHITE_MIST_FILLS_THE_ROOM_AND_RISES1 ) SOFT_NL 
             GET_CSTR( CSTR_FED_BY_STREAM_WHICH_TUMBLES_OUT_OF_HOLE1 ) SOFT_NL 
             GET_CSTR( CSTR_OVERHEAD_AND_SPLASHES_NOISILY_INTO1 ) SOFT_NL 
             GET_CSTR( CSTR_MIST_THE_ONLY_PASSAGE_GOES_BACK_TOWARD1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_RESERVOIR ), 0); 
    make_ins(S, R_MIRROR); ditto(OUT); 
 
    /* R_SCAN1 and R_SCAN3 are the rooms the player sees when entering the 
     * secret canyon from the north and the east, respectively. The dragon 
     * blocks different exits in each room (and items dropped at one end of 
     * the canyon are not visible or accessible from the other end). 
     * Once the dragon has been vanquished, R_SCAN2 replaces both rooms. 
     */ 
    make_loc(q, R_SCAN1, 
             GET_CSTR( CSTR_YOU_ARE_IN_SECRET_CANYON_THAT_EXITS_TO1 ), 
             NULL, 0); 
    make_ins(N, R_ABOVER); ditto(OUT); 
    make_ins(E, remark(10)); ditto(FORWARD); 
    make_loc(q, R_SCAN2, 
             places[R_SCAN1].long_desc, 
             NULL, 0); 
    make_ins(N, R_ABOVER); 
    make_ins(E, R_SECRET); 
    make_loc(q, R_SCAN3, 
             places[R_SCAN1].long_desc, 
             NULL, 0); 
    make_ins(E, R_SECRET); ditto(OUT); 
    make_ins(N, remark(10)); ditto(FORWARD); 
 
    make_loc(q, R_SECRET, 
             GET_CSTR( CSTR_YOU_ARE_IN_SECRET_CANYON_WHICH_HERE1 ) SOFT_NL 
             GET_CSTR( CSTR_VERY_TIGHT_CANYON_FEET_BELOW_IF_YOU_GO1 ) SOFT_NL 
             GET_CSTR( CSTR_TO_GET_BACK_UP ), 
             GET_CSTR( CSTR_YOU_RE_IN_SECRET_CANYON_ABOVE_TIGHT1 ), 0); 
    make_ins(E, R_HMK); 
    make_cond_ins(W, unless_prop(DRAGON, 0), R_SCAN2); 
    make_ins(W, R_SCAN3); 
    make_ins(D, R_WIDE); 
    make_loc(q, R_WIDE, 
             GET_CSTR( CSTR_YOU_ARE_AT_WIDE_PLACE_IN_VERY_TIGHT1 ), 
             NULL, 0); 
    make_ins(S, R_TIGHT); 
    make_ins(N, R_TALL); 
    make_loc(q, R_TIGHT, 
             GET_CSTR( CSTR_THE_CANYON_HERE_BECOMES_TOO_TIGHT_TO_GO1 ), 
             NULL, 0); 
    make_ins(N, R_WIDE); 
    make_loc(q, R_TALL, 
             GET_CSTR( CSTR_YOU_ARE_IN_TALL_CANYON_LOW_TIGHT_CRAWL1 ) SOFT_NL 
             GET_CSTR( CSTR_SEEMS_TO_OPEN_UP ), 
             GET_CSTR( CSTR_YOU_RE_IN_TALL_CANYON1 ), 0); 
    make_ins(E, R_WIDE); 
    make_ins(W, R_BOULDERS); 
    make_ins(N, R_SWISS); ditto(CRAWL); 
    make_loc(q, R_BOULDERS, 
             GET_CSTR( CSTR_THE_CANYON_RUNS_INTO_MASS_OF_BOULDERS1 ) EMDASH("--") GET_CSTR( CSTR_DEAD_END1 ), 
             NULL, 0); 
    make_ins(S, R_TALL); 
    make_loc(q, R_SLOPING, 
             GET_CSTR( CSTR_YOU_ARE_IN_LONG_WINDING_CORRIDOR1 ) SOFT_NL 
             GET_CSTR( CSTR_DIRECTIONS ), 
             GET_CSTR( CSTR_YOU_RE_IN_SLOPING_CORRIDOR1 ), 0); 
    make_ins(D, R_LOW); 
    make_ins(U, R_SWSIDE); 
    make_loc(q, R_SWSIDE, 
             GET_CSTR( CSTR_YOU_ARE_ON_ONE_SIDE_OF_LARGE_DEEP_CHASM1 ) SOFT_NL 
             GET_CSTR( CSTR_UP_FROM_BELOW_OBSCURES_ALL_VIEW_OF_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_FROM_THE_CHASM_INTO_WINDING_CORRIDOR1 ), 
             GET_CSTR( CSTR_YOU_RE_ON_SW_SIDE_OF_CHASM1 ), 0); 
    make_ins(SW, R_SLOPING); 
    make_cond_ins(OVER, only_if_here(TROLL), remark(11)); ditto(ACROSS); ditto(CROSS); ditto(NE); 
    make_cond_ins(OVER, unless_prop(CHASM, 0), remark(12)); 
    make_ins(OVER, R_TROLL); 
    make_cond_ins(JUMP, unless_prop(CHASM, 0), R_LOSE); 
    make_ins(JUMP, remark(2)); 
    make_loc(q, R_DEAD0, dead_end, NULL, 0); 
    make_ins(S, R_CROSS); ditto(OUT); 
    make_loc(q, R_DEAD1, dead_end, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE11); ditto(OUT); 
    make_loc(q, R_PIRATES_NEST, dead_end, NULL, 0); 
    make_ins(SE, R_LIKE13); 
    make_loc(q, R_DEAD3, dead_end, NULL, F_TWIST_HINT); 
    make_ins(W, R_LIKE4); ditto(OUT); 
    make_loc(q, R_DEAD4, dead_end, 0, F_TWIST_HINT); 
    make_ins(E, R_LIKE4); ditto(OUT); 
    make_loc(q, R_DEAD5, dead_end, 0, F_TWIST_HINT); 
    make_ins(U, R_LIKE3); ditto(OUT); 
    make_loc(q, R_DEAD6, dead_end, 0, F_TWIST_HINT); 
    make_ins(W, R_LIKE9); ditto(OUT); 
    make_loc(q, R_DEAD7, dead_end, 0, F_TWIST_HINT); 
    make_ins(U, R_LIKE10); ditto(OUT); 
    make_loc(q, R_DEAD8, dead_end, 0, 0); 
    make_ins(E, R_BRINK); ditto(OUT); 
    make_loc(q, R_DEAD9, dead_end, 0, F_TWIST_HINT); 
    make_ins(S, R_LIKE3); ditto(OUT); 
    make_loc(q, R_DEAD10, dead_end, 0, F_TWIST_HINT); 
    make_ins(E, R_LIKE12); ditto(OUT); 
    make_loc(q, R_DEAD11, dead_end, 0, F_TWIST_HINT); 
    make_ins(U, R_LIKE8); ditto(OUT); 
    make_loc(q, R_NESIDE, 
             GET_CSTR( CSTR_YOU_ARE_ON_THE_FAR_SIDE_OF_THE_CHASM_NE1 ) SOFT_NL 
             GET_CSTR( CSTR_CHASM_ON_THIS_SIDE ), 
             GET_CSTR( CSTR_YOU_RE_ON_NE_SIDE_OF_CHASM1 ), 0); 
    make_ins(NE, R_CORR); 
    make_cond_ins(OVER, only_if_here(TROLL), remark(11)); ditto(ACROSS); ditto(CROSS); ditto(SW); 
    make_ins(OVER, R_TROLL); 
    make_ins(JUMP, remark(2)); 
    make_ins(FORK, R_FORK); 
    make_ins(VIEW, R_VIEW); 
    make_ins(BARREN, R_FBARR); 
    make_loc(q, R_CORR, 
             GET_CSTR( CSTR_YOU_RE_IN_LONG_EAST_WEST_CORRIDOR_FAINT1 ) SOFT_NL 
             GET_CSTR( CSTR_HEARD_IN_THE_DISTANCE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_CORRIDOR ), 0); 
    make_ins(W, R_NESIDE); 
    make_ins(E, R_FORK); ditto(FORK); 
    make_ins(VIEW, R_VIEW); 
    make_ins(BARREN, R_FBARR); 
    make_loc(q, R_FORK, 
             GET_CSTR( CSTR_THE_PATH_FORKS_HERE_THE_LEFT_FORK_LEADS1 ) SOFT_NL 
             GET_CSTR( CSTR_SEEMS_TO_GET_LOUDER_IN_THAT_DIRECTION1 ) SOFT_NL 
             GET_CSTR( CSTR_DOWN_GENTLE_SLOPE_THE_MAIN_CORRIDOR1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_FORK_IN_PATH1 ), 0); 
    make_ins(W, R_CORR); 
    make_ins(NE, R_WARM); ditto(LEFT); 
    make_ins(SE, R_LIME); ditto(RIGHT); ditto(D); 
    make_ins(VIEW, R_VIEW); 
    make_ins(BARREN, R_FBARR); 
    make_loc(q, R_WARM, 
             GET_CSTR( CSTR_THE_WALLS_ARE_QUITE_WARM_HERE_FROM_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_ROAR_SO_LOUD_THAT_THE_ENTIRE_CAVE_SEEMS1 ) SOFT_NL 
             GET_CSTR( CSTR_PASSAGE_LEADS_SOUTH_AND_LOW_CRAWL_GOES1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_JUNCTION_WITH_WARM_WALLS1 ), 0); 
    make_ins(S, R_FORK); ditto(FORK); 
    make_ins(N, R_VIEW); ditto(VIEW); 
    make_ins(E, R_CHAMBER); ditto(CRAWL); 
    make_loc(q, R_VIEW, 
             GET_CSTR( CSTR_YOU_ARE_ON_THE_EDGE_OF_BREATH_TAKING1 ) SOFT_NL 
             GET_CSTR( CSTR_ACTIVE_VOLCANO_FROM_WHICH_GREAT_GOUTS1 ) SOFT_NL 
             GET_CSTR( CSTR_OUT_CASCADING_BACK_DOWN_INTO_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_FARTHEST_REACHES_OF_THE_CAVERN_WITH1 ) SOFT_HYPHEN 
             GET_CSTR( CSTR_THING_AN_EERIE_MACABRE_APPEARANCE_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_SPARKS_OF_ASH_AND_HEAVY_SMELL_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_THE_TOUCH_AND_THE_THUNDERING_OF_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_SOUNDS_EMBEDDED_IN_THE_JAGGED_ROOF_FAR1 ) SOFT_NL 
             GET_CSTR( CSTR_FORMATIONS_COMPOSED_OF_PURE_WHITE1 ) SOFT_NL 
             GET_CSTR( CSTR_LIGHT_INTO_SINISTER_APPARITIONS_UPON1 ) SOFT_NL 
             GET_CSTR( CSTR_GORGE_FILLED_WITH_BIZARRE_CHAOS_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_BEEN_CRAFTED_BY_THE_DEVIL_HIMSELF_AN1 ) SOFT_NL 
             GET_CSTR( CSTR_OUT_FROM_THE_DEPTHS_OF_THE_VOLCANO1 ) SOFT_NL 
             GET_CSTR( CSTR_AND_PLUMMETS_INTO_BOTTOMLESS_PIT_FAR1 ) SOFT_NL 
             GET_CSTR( CSTR_RIGHT_AN_IMMENSE_GEYSER_OF_BLISTERING1 ) SOFT_NL 
             GET_CSTR( CSTR_FROM_BARREN_ISLAND_IN_THE_CENTER_OF1 ) SOFT_NL 
             GET_CSTR( CSTR_OMINOUSLY_THE_FAR_RIGHT_WALL_IS_AFLAME1 ) SOFT_NL 
             GET_CSTR( CSTR_OWN_WHICH_LENDS_AN_ADDITIONAL1 ) SOFT_NL 
             GET_CSTR( CSTR_HELLISH_SCENE_DARK_FOREBODING1 ), 
             GET_CSTR( CSTR_YOU_RE_AT_BREATH_TAKING_VIEW1 ), 0); 
    make_ins(S, R_WARM); ditto(PASSAGE); ditto(OUT); 
    make_ins(FORK, R_FORK); 
    make_ins(D, remark(13)); ditto(JUMP); 
    make_loc(q, R_CHAMBER, 
             GET_CSTR( CSTR_YOU_ARE_IN_SMALL_CHAMBER_FILLED_WITH1 ) SOFT_NL 
             GET_CSTR( CSTR_VERY_WARM_CAUSING_THE_AIR_IN_THE_ROOM_TO1 ) SOFT_NL 
             GET_CSTR( CSTR_HEAT_THE_ONLY_EXIT_IS_CRAWL_HEADING1 ) SOFT_NL 
             GET_CSTR( CSTR_LOW_RUMBLING ), 
             GET_CSTR( CSTR_YOU_RE_IN_CHAMBER_OF_BOULDERS1 ), 0); 
    make_ins(W, R_WARM); ditto(OUT); ditto(CRAWL); 
    make_ins(FORK, R_FORK); 
    make_ins(VIEW, R_VIEW); 
    make_loc(q, R_LIME, 
             GET_CSTR( CSTR_YOU_ARE_WALKING_ALONG_GENTLY_SLOPING1 ) SOFT_NL 
             GET_CSTR( CSTR_ODDLY_SHAPED_LIMESTONE_FORMATIONS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_LIMESTONE_PASSAGE1 ), 0); 
    make_ins(N, R_FORK); ditto(U); ditto(FORK); 
    make_ins(S, R_FBARR); ditto(D); ditto(BARREN); 
    make_ins(VIEW, R_VIEW); 
    make_loc(q, R_FBARR, 
             GET_CSTR( CSTR_YOU_ARE_STANDING_AT_THE_ENTRANCE_TO1 ) SOFT_NL 
             GET_CSTR( CSTR_POSTED_ABOVE_THE_ENTRANCE_READS1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_FRONT_OF_BARREN_ROOM1 ), 0); 
    make_ins(W, R_LIME); ditto(U); 
    make_ins(FORK, R_FORK); 
    make_ins(E, R_BARR); ditto(IN); ditto(BARREN); ditto(ENTER); 
    make_ins(VIEW, R_VIEW); 
    make_loc(q, R_BARR, 
             GET_CSTR( CSTR_YOU_ARE_INSIDE_BARREN_ROOM_THE_CENTER1 ) SOFT_NL 
             GET_CSTR( CSTR_EMPTY_EXCEPT_FOR_SOME_DUST_MARKS_IN_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_FAR_END_OF_THE_ROOM_THE_ONLY_EXIT_IS_THE1 ), 
             GET_CSTR( CSTR_YOU_RE_IN_BARREN_ROOM1 ), 0); 
    make_ins(W, R_FBARR); ditto(OUT); 
    make_ins(FORK, R_FORK); 
    make_ins(VIEW, R_VIEW); 
    /* The end-game repository. */ 
    make_loc(q, R_NEEND, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_NORTHEAST_END_OF_AN1 ) SOFT_NL 
             GET_CSTR( CSTR_GIANT_ROOM_IT_APPEARS_TO_BE_REPOSITORY1 ) SOFT_NL 
             GET_CSTR( CSTR_PROGRAM_MASSIVE_TORCHES_FAR_OVERHEAD ) SOFT_NL 
             GET_CSTR( CSTR_YELLOW_LIGHT_SCATTERED_ABOUT_YOU_CAN ) SOFT_NL 
             GET_CSTR( CSTR_OF_THEM_EMPTY_NURSERY_OF_YOUNG ) SOFT_NL 
             GET_CSTR( CSTR_OF_OYSTERS_BUNDLE_OF_BLACK_RODS_WITH ) SOFT_NL 
             GET_CSTR( CSTR_COLLECTION_OF_BRASS_LANTERNS_OFF_TO ) SOFT_NL 
             GET_CSTR( CSTR_ARE_SLEEPING_ON_THE_FLOOR_SNORING ) SOFT_NL 
             GET_CSTR( CSTR_NOT_DISTURB_THE_DWARVES_AN_IMMENSE ) SOFT_NL 
             GET_CSTR( CSTR_WALL_AND_STRETCHES_TO_THE_OTHER_END_OF ) SOFT_NL 
             GET_CSTR( CSTR_SUNDRY_OBJECTS_CAN_BE_GLIMPSED_DIMLY ), 
             GET_CSTR( CSTR_YOU_RE_AT_NE_END ), 0); 
    make_ins(SW, R_SWEND); 
    /* The following description has several minor differences from Woods' original. 
     * Woods' line breaks come after "A" on lines 4 and 5, "large" on line 6, and 
     * "vault" on line 7. Knuth's "that reads" corresponds to Woods' "which reads"; 
     * presumably Knuth changed it to avoid ugly repetition, and I agree. */ 
    make_loc(q, R_SWEND, 
             GET_CSTR( CSTR_YOU_ARE_AT_THE_SOUTHWEST_END_OF_THE ) SOFT_NL 
             GET_CSTR( CSTR_FULL_OF_FIERCE_GREEN_SNAKES_ON_THE ) SOFT_NL 
             GET_CSTR( CSTR_WICKER_CAGES_EACH_OF_WHICH_CONTAINS ) SOFT_NL 
             GET_CSTR( CSTR_CORNER_IS_BUNDLE_OF_BLACK_RODS_WITH ) SOFT_NL 
             GET_CSTR( CSTR_LARGE_NUMBER_OF_VELVET_PILLOWS_ARE ) SOFT_NL 
             GET_CSTR( CSTR_VAST_MIRROR_STRETCHES_OFF_TO_THE ) SOFT_NL 
             GET_CSTR( CSTR_LARGE_STEEL_GRATE_NEXT_TO_WHICH_IS_SIGN ) SOFT_NL 
             GET_CSTR( CSTR_VAULT_KEYS_IN_MAIN_OFFICE ), 
             GET_CSTR( CSTR_YOU_RE_AT_SW_END ), 0); 
    make_ins(NE, R_NEEND); 
    make_ins(D, remark(1));  /* You can't go through a locked steel grate! */ 
 
    /* The following pseudo-locations have "forced" movement. 
     * In such cases we don't ask you for input; we assume that you have told 
     * us to force another instruction through. For example, if you try to 
     * JUMP across the fissure (R_EFISS), the instruction there sends you to 
     * R_LOSE, which prints the room description ("You didn't make it.") and 
     * immediately sends you to R_LIMBO, i.e., death. 
     * Crowther (and therefore Woods and therefore Knuth) implemented several 
     * responses as pseudo-locations; for example, "The dome is unclimbable" 
     * and "The crack is far too small for you to follow". For the ones that 
     * behave indistinguishably from remarks, I've converted them to remarks. 
     */ 
    make_loc(q, R_NECK, GET_CSTR( CSTR_YOU_ARE_AT_THE_BOTTOM_OF_THE_PIT_WITH ), NULL, 0); 
    make_ins(0, R_LIMBO); 
    make_loc(q, R_LOSE, GET_CSTR( CSTR_YOU_DIDN_MAKE_IT ), NULL, 0); 
    make_ins(0, R_LIMBO); 
    make_loc(q, R_CLIMB, GET_CSTR( CSTR_YOU_CLAMBER_UP_THE_PLANT_AND_SCURRY ), NULL, 0); 
    make_ins(0, R_NARROW); 
    /* Typing CLIMB from the bottom of the west pit triggers a clever bit 
     * of gymnastics. We want to branch three ways on the state of the 
     * plant (too small to climb; "up the plant and out of the pit"; and 
     * all the way up the beanstalk). But the only operation available to 
     * us is "travel if objs(PLANT).prop is NOT x". So R_WPIT's instruction 
     * brings us to R_CHECK if objs(PLANT).prop != 2, and R_CHECK dispatches 
     * to one of the two non-narrow-corridor locations. */ 
    make_loc(q, R_CHECK, NULL, NULL, 0); 
    make_cond_ins(0, unless_prop(PLANT, 1), R_UPNOUT); 
    make_ins(0, R_DIDIT); 
    make_loc(q, R_THRU, 
             GET_CSTR( CSTR_YOU_HAVE_CRAWLED_THROUGH_VERY_LOW_WIDE ) SOFT_NL 
             GET_CSTR( CSTR_OF_THE_HALL_OF_MISTS ), 
             NULL, 0); 
    make_ins(0, R_WMIST); 
    make_loc(q, R_DUCK, places[R_THRU].long_desc, NULL, 0); 
    make_ins(0, R_WFISS); 
    make_loc(q, R_UPNOUT, 
             GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_TO_CLIMB_USE_UP_OR ), 
             NULL, 0); 
    make_ins(0, R_WPIT); 
    make_loc(q, R_DIDIT, GET_CSTR( CSTR_YOU_HAVE_CLIMBED_UP_THE_PLANT_AND_OUT_OF ), NULL, 0); 
    make_ins(0, R_W2PIT); 
 
    /* The remaining "locations" R_PPASS, R_PDROP, and R_TROLL are special. */ 
    start[R_PPASS] = q; 
} 
 
bool is_forced(Location loc) 
{ 
    switch (loc) { 
        case R_NECK: 
        case R_LOSE: 
        case R_CLIMB: 
        case R_CHECK: 
        case R_THRU: 
        case R_DUCK: 
        case R_UPNOUT: 
        case R_DIDIT: 
            return true; 
        default: 
            return false; 
    } 
} 
 
bool has_light(Location loc) 
{ 
    switch (loc) { 
        case R_ROAD: case R_HILL: case R_HOUSE: case R_VALLEY: 
        case R_FOREST: case R_FOREST2: case R_SLIT: case R_OUTSIDE: 
        case R_INSIDE: case R_COBBLES: 
        case R_PLOVER: case R_VIEW: 
        case R_NEEND: case R_SWEND: 
            return true; 
        default: 
            return false; 
    } 
} 
 
bool has_water(Location loc) 
{ 
    switch (loc) { 
        case R_ROAD: case R_HOUSE: case R_VALLEY: case R_SLIT: 
        case R_WET: case R_FALLS: case R_RES: 
            return true; 
        default: 
            return false; 
    } 
} 
 
bool has_oil(Location loc) 
{ 
    return (loc == R_EPIT); 
} 
 
/*========== Data structures for objects. ================================= 
 * This section corresponds to sections 63--70 in Knuth. 
 */ 
 
struct ObjectData { 
    ObjectWord link;  /* next object at this location, or NOTHING */ 
    ObjectWord base;  /* NOTHING for mobile objects */ 
    int prop; 
    Location place; 
    const char *name; 
    const char *desc[4];  /* .prop ranges from 0 to 3 */ 
} objs_[MAX_OBJ+1 - MIN_OBJ]; 
#define objs(t) objs_[(t)-MIN_OBJ] 
 
int holding_count;  /* how many objects have objs(t).place < 0? */ 
Location last_knife_loc = R_LIMBO; 
int tally = 15;  /* treasures awaiting you */ 
int lost_treasures;  /* treasures that you won't find */ 
 
#define toting(t) (objs(t).place < 0) 
#define is_immobile(t) (objs(t).base != NOTHING) 
#define there(t, loc) (objs(t).place == (loc)) 
 
/* Return true if t is a treasure. Notice that RUG_ (the other half 
 * of the schizoid rug) does not need to be a treasure. You can 
 * never be carrying it; the pirate can't steal it; and its prop 
 * value is irrelevant (we use the prop value of the base object RUG). 
 */ 
bool is_treasure(ObjectWord t) 
{ 
    switch (t) { 
        case GOLD: case DIAMONDS: case SILVER: case JEWELS: 
        case COINS: case CHEST: case EGGS: case TRIDENT: 
        case VASE: case EMERALD: case PYRAMID: case PEARL: 
        case RUG: case SPICES: case CHAIN: 
            return true; 
        default: 
            return false; 
    } 
} 
 
ObjectWord bottle_contents(void) 
{ 
    switch (objs(BOTTLE).prop) { 
        case 0: return WATER; 
        case 2: return OIL; 
        /* other valid values: 1, -2 (post-closing) */ 
    } 
    return NOTHING; 
} 
 
/* Return true if t is at loc, or being carried. */ 
bool here(ObjectWord t, Location loc) 
{ 
    return toting(t) || there(t, loc); 
} 
 
void drop(ObjectWord t, Location l) 
{ 
    assert(objs(t).place == R_INHAND || objs(t).place == R_LIMBO); 
    assert(objs(t).link == NOTHING); 
    if (toting(t)) --holding_count; 
    objs(t).place = l; 
    if (l == R_INHAND) { 
        ++holding_count; 
    } else if (l != R_LIMBO) { 
        objs(t).link = places[l].objects; 
        places[l].objects = t; 
    } 
} 
 
#define move(t,l) do { carry(t); drop((t),l); } while (0) 
#define juggle(t) do { Location l = objs(t).place; move(t, l); } while (0) 
#define destroy(t) move((t), R_LIMBO) 
 
void carry(ObjectWord t) 
{ 
    Location l = objs(t).place; 
    if (l != R_INHAND) { 
        if (l > R_LIMBO) { 
            /* Remove t from l's object-list */ 
            ObjectWord *p = &places[l].objects; 
            while (*p != t) p = &objs(*p).link; 
            *p = objs(*p).link; 
        } 
        objs(t).place = R_INHAND; 
        objs(t).link = NOTHING; 
        ++holding_count; 
    } 
} 
 
bool is_at_loc(ObjectWord t, Location loc) 
{ 
    if (objs(t).base == NOTHING) 
        return there(t, loc); 
    /* Check the "alternative" objects based on this one. */ 
    for (ObjectWord tt = t; objs(tt).base == t; ++tt) { 
        if (there(tt, loc)) 
            return true; 
    } 
    return false; 
} 
 
void mobilize(ObjectWord t) { objs(t).base = NOTHING; } 
void immobilize(ObjectWord t) { objs(t).base = t; } 
 
void new_obj(ObjectWord t, const char *n, ObjectWord b, Location l) 
{ 
    objs(t).name = n; 
    objs(t).prop = (is_treasure(t) ? -1 : 0); 
    objs(t).base = b; 
    objs(t).place = l; 
    objs(t).link = NOTHING; 
    if (l > R_LIMBO) { 
       /* Drop the object at the *end* of its list. Combined with the 
         * ordering of the item numbers, this ensures that the CHASM 
         * is described before the TROLL, the DRAGON before the RUG, 
         * and so on. */ 
        ObjectWord *p = &places[l].objects; 
        while (*p != NOTHING) 
            p = &objs(*p).link; 
        *p = t; 
    } 
} 
 
void build_object_table(void) 
{ 
    new_obj(KEYS, GET_CSTR( CSTR_SET_OF_KEYS ), 0, R_HOUSE); 
    objs(KEYS).desc[0] = GET_CSTR( CSTR_THERE_ARE_SOME_KEYS_ON_THE_GROUND_HERE ); 
    new_obj(LAMP, GET_CSTR( CSTR_BRASS_LANTERN ), 0, R_HOUSE); 
    objs(LAMP).desc[0] = GET_CSTR( CSTR_THERE_IS_SHINY_BRASS_LAMP_NEARBY ); 
    objs(LAMP).desc[1] = GET_CSTR( CSTR_THERE_IS_LAMP_SHINING_NEARBY ); 
    new_obj(GRATE, 0, GRATE, R_OUTSIDE); 
    new_obj(GRATE_, 0, GRATE, R_INSIDE); 
    objs(GRATE).desc[0] = GET_CSTR( CSTR_THE_GRATE_IS_LOCKED ); 
    objs(GRATE).desc[1] = GET_CSTR( CSTR_THE_GRATE_IS_OPEN ); 
    new_obj(CAGE, GET_CSTR( CSTR_WICKER_CAGE ), 0, R_COBBLES); 
    objs(CAGE).desc[0] = GET_CSTR( CSTR_THERE_IS_SMALL_WICKER_CAGE_DISCARDED ); 
    new_obj(ROD, GET_CSTR( CSTR_BLACK_ROD ), 0, R_DEBRIS); 
    objs(ROD).desc[0] = GET_CSTR( CSTR_THREE_FOOT_BLACK_ROD_WITH_RUSTY_STAR_ON ); 
    new_obj(ROD2, GET_CSTR( CSTR_BLACK_ROD ), 0, R_LIMBO); 
    objs(ROD2).desc[0] = GET_CSTR( CSTR_THREE_FOOT_BLACK_ROD_WITH_RUSTY_MARK_ON ); 
    new_obj(TREADS, 0, TREADS, R_SPIT); 
    new_obj(TREADS_, 0, TREADS, R_EMIST); 
    objs(TREADS).desc[0] = GET_CSTR( CSTR_ROUGH_STONE_STEPS_LEAD_DOWN_THE_PIT ); 
    objs(TREADS).desc[1] = GET_CSTR( CSTR_ROUGH_STONE_STEPS_LEAD_UP_THE_DOME1 ); 
    new_obj(BIRD, GET_CSTR( CSTR_LITTLE_BIRD_IN_CAGE ), 0, R_BIRD); 
    objs(BIRD).desc[0] = GET_CSTR( CSTR_CHEERFUL_LITTLE_BIRD_IS_SITTING_HERE ); 
    objs(BIRD).desc[1] = GET_CSTR( CSTR_THERE_IS_LITTLE_BIRD_IN_THE_CAGE1 ); 
    new_obj(RUSTY_DOOR, 0, RUSTY_DOOR, R_IMMENSE); 
    objs(RUSTY_DOOR).desc[0] = GET_CSTR( CSTR_THE_WAY_NORTH_IS_BARRED_BY_MASSIVE ); 
    objs(RUSTY_DOOR).desc[1] = GET_CSTR( CSTR_THE_WAY_NORTH_LEADS_THROUGH_MASSIVE ); 
    new_obj(PILLOW, GET_CSTR( CSTR_VELVET_PILLOW ), 0, R_SOFT); 
    objs(PILLOW).desc[0] = GET_CSTR( CSTR_SMALL_VELVET_PILLOW_LIES_ON_THE_FLOOR1 ); 
    new_obj(SNAKE, 0, SNAKE, R_HMK); 
    objs(SNAKE).desc[0] = GET_CSTR( CSTR_HUGE_GREEN_FIERCE_SNAKE_BARS_THE_WAY ); 
    objs(SNAKE).desc[1] = NULL;  /* chased away */ 
    new_obj(FISSURE, 0, FISSURE, R_EFISS); 
    new_obj(FISSURE_, 0, FISSURE, R_WFISS); 
    objs(FISSURE).desc[0] = NULL; 
    objs(FISSURE).desc[1] =GET_CSTR( CSTR_CRYSTAL_BRIDGE_NOW_SPANS_THE_FISSURE ); 
    new_obj(TABLET, 0, TABLET, R_DARK); 
    objs(TABLET).desc[0] =  /* Woods has "imbedded". */ 
        GET_CSTR( CSTR_MASSIVE_STONE_TABLET_EMBEDDED_IN_THE ) SOFT_NL 
        GET_CSTR( CSTR_CONGRATULATIONS_ON_BRINGING_LIGHT2 ); 
    new_obj(CLAM, GET_CSTR( CSTR_GIANT_CLAM_GRUNT ), 0, R_SHELL); 
    objs(CLAM).desc[0] = GET_CSTR( CSTR_THERE_IS_AN_ENORMOUS_CLAM_HERE_WITH_ITS ); 
    new_obj(OYSTER, GET_CSTR( CSTR_GIANT_OYSTER_GROAN1 ), 0, R_LIMBO); 
    objs(OYSTER).desc[0] = GET_CSTR( CSTR_THERE_IS_AN_ENORMOUS_OYSTER_HERE_WITH1 ); 
    new_obj(MAG, GET_CSTR( CSTR_SPELUNKER_TODAY ), 0, R_ANTE); 
    objs(MAG).desc[0] = GET_CSTR( CSTR_THERE_ARE_FEW_RECENT_ISSUES_OF1 ); 
    new_obj(DWARF, 0, DWARF, R_LIMBO); 
    new_obj(KNIFE, 0, 0, R_LIMBO); 
    new_obj(FOOD, GET_CSTR( CSTR_TASTY_FOOD ), 0, R_HOUSE); 
    objs(FOOD).desc[0] = GET_CSTR( CSTR_THERE_IS_FOOD_HERE ); 
    new_obj(BOTTLE, GET_CSTR( CSTR_SMALL_BOTTLE ), 0, R_HOUSE); 
    objs(BOTTLE).desc[0] = GET_CSTR( CSTR_THERE_IS_BOTTLE_OF_WATER_HERE1 ); 
    objs(BOTTLE).desc[1] = GET_CSTR( CSTR_THERE_IS_AN_EMPTY_BOTTLE_HERE1 ); 
    objs(BOTTLE).desc[2] = GET_CSTR( CSTR_THERE_IS_BOTTLE_OF_OIL_HERE ); 
    /* WATER and OIL never appear on the ground; they are invariably 
     * either in R_LIMBO or R_INHAND. */ 
    new_obj(WATER, GET_CSTR( CSTR_WATER_IN_THE_BOTTLE ), 0, R_LIMBO); 
    new_obj(OIL, GET_CSTR( CSTR_OIL_IN_THE_BOTTLE ), 0, R_LIMBO); 
    new_obj(MIRROR, 0, MIRROR, R_MIRROR); 
    new_obj(MIRROR_, 0, MIRROR, R_LIMBO);  /* joins up with MIRROR later */ 
    objs(MIRROR).desc[0] = NULL; 
    new_obj(PLANT, 0, PLANT, R_WPIT); 
    objs(PLANT).desc[0] = GET_CSTR( CSTR_THERE_IS_TINY_LITTLE_PLANT_IN_THE_PIT1 ); 
    objs(PLANT).desc[1] = 
        GET_CSTR( CSTR_THERE_IS_FOOT_TALL_BEANSTALK1 ) SOFT_NL 
        GET_CSTR( CSTR_BELLOWING_WATER_WATER1 ); 
    objs(PLANT).desc[2] = GET_CSTR( CSTR_THERE_IS_GIGANTIC_BEANSTALK1 ); 
    new_obj(PLANT2, 0, PLANT2, R_W2PIT); 
    new_obj(PLANT2_, 0, PLANT2, R_E2PIT); 
    objs(PLANT2).desc[0] = NULL; 
    objs(PLANT2).desc[1] = GET_CSTR( CSTR_THE_TOP_OF_FOOT_TALL_BEANSTALK_IS1 ); 
    objs(PLANT2).desc[2] = GET_CSTR( CSTR_THERE_IS_HUGE_BEANSTALK_GROWING_OUT_OF1 ); 
    new_obj(STALACTITE, 0, STALACTITE, R_TITE); 
    objs(STALACTITE).desc[0] = NULL; 
    new_obj(SHADOW, 0, SHADOW, R_WINDOE); 
    new_obj(SHADOW_, 0, SHADOW, R_WINDOW); 
    objs(SHADOW).desc[0] = GET_CSTR( CSTR_THE_SHADOWY_FIGURE_SEEMS_TO_BE_TRYING1 ); 
    new_obj(AXE, GET_CSTR( CSTR_DWARF_AXE ), 0, R_LIMBO); 
    objs(AXE).desc[0] = GET_CSTR( CSTR_THERE_IS_LITTLE_AXE_HERE1 ); 
    objs(AXE).desc[1] = GET_CSTR( CSTR_THERE_IS_LITTLE_AXE_LYING_BESIDE_THE1 ); 
    new_obj(DRAWINGS, 0, DRAWINGS, R_ORIENTAL); 
    objs(DRAWINGS).desc[0] = NULL; 
    new_obj(PIRATE, 0, PIRATE, R_LIMBO); 
    new_obj(DRAGON, 0, DRAGON, R_SCAN1); 
    new_obj(DRAGON_, 0, DRAGON, R_SCAN3); 
    objs(DRAGON).desc[0] = GET_CSTR( CSTR_HUGE_GREEN_FIERCE_DRAGON_BARS_THE_WAY1 ); 
    objs(DRAGON).desc[1] = GET_CSTR( CSTR_THE_BODY_OF_HUGE_GREEN_DEAD_DRAGON_IS1 ); 
    new_obj(CHASM, 0, CHASM, R_SWSIDE); 
    new_obj(CHASM_, 0, CHASM, R_NESIDE); 
    objs(CHASM).desc[0] = 
        GET_CSTR( CSTR_RICKETY_WOODEN_BRIDGE_EXTENDS_ACROSS1 ) SOFT_NL 
        GET_CSTR( CSTR_MIST_SIGN_POSTED_ON_THE_BRIDGE_READS1 ); 
    objs(CHASM).desc[1] = 
        GET_CSTR( CSTR_THE_WRECKAGE_OF_BRIDGE_AND_DEAD_BEAR1 ) SOFT_NL 
        GET_CSTR( CSTR_OF_THE_CHASM ); 
    new_obj(TROLL, 0, TROLL, R_SWSIDE); 
    new_obj(TROLL_, 0, TROLL, R_NESIDE); 
    objs(TROLL).desc[0] = 
        GET_CSTR( CSTR_BURLY_TROLL_STANDS_BY_THE_BRIDGE_AND1 ) SOFT_NL 
        GET_CSTR( CSTR_TREASURE_BEFORE_YOU_MAY_CROSS1 ); 
    objs(TROLL).desc[1] = NULL;  /* not present, but not paid off either */ 
    objs(TROLL).desc[2] = NULL;  /* chased away */ 
    new_obj(NO_TROLL, 0, NO_TROLL, R_LIMBO); 
    new_obj(NO_TROLL_, 0, NO_TROLL, R_LIMBO); 
    objs(NO_TROLL).desc[0] = GET_CSTR( CSTR_THE_TROLL_IS_NOWHERE_TO_BE_SEEN1 ); 
    new_obj(BEAR, 0, BEAR, R_BARR); 
    objs(BEAR).desc[0] = GET_CSTR( CSTR_THERE_IS_FEROCIOUS_CAVE_BEAR_EYING_YOU1 ); 
    objs(BEAR).desc[1] = GET_CSTR( CSTR_THERE_IS_GENTLE_CAVE_BEAR_SITTING1 ); 
    objs(BEAR).desc[2] = GET_CSTR( CSTR_THERE_IS_CONTENTED_LOOKING_BEAR1 ); 
    objs(BEAR).desc[3] = NULL;  /* the dead bear remains as scenery where it fell */ 
    new_obj(MESSAGE, 0, MESSAGE, R_LIMBO); 
    objs(MESSAGE).desc[0] = 
        GET_CSTR( CSTR_THERE_IS_MESSAGE_SCRAWLED_IN_THE_DUST1 ) SOFT_NL 
        GET_CSTR( CSTR_THIS_IS_NOT_THE_MAZE_WHERE_THE_PIRATE2 ); 
    new_obj(GORGE, 0, GORGE, R_VIEW); 
    objs(GORGE).desc[0] = NULL;  /* it's just scenery */ 
    new_obj(MACHINE, 0, MACHINE, R_PONY); 
    objs(MACHINE).desc[0] = 
        GET_CSTR( CSTR_THERE_IS_MASSIVE_VENDING_MACHINE_HERE1 ) SOFT_NL 
        GET_CSTR( CSTR_DROP_COINS_HERE_TO_RECEIVE_FRESH1 ); 
    new_obj(BATTERIES, GET_CSTR( CSTR_BATTERIES ), 0, R_LIMBO); 
    objs(BATTERIES).desc[0] = GET_CSTR( CSTR_THERE_ARE_FRESH_BATTERIES_HERE3 ); 
    objs(BATTERIES).desc[1] = GET_CSTR( CSTR_SOME_WORN_OUT_BATTERIES_HAVE_BEEN1 ); 
    new_obj(MOSS, 0, MOSS, R_SOFT); 
    objs(MOSS).desc[0] = NULL;  /* it's just scenery */ 
    new_obj(GOLD, GET_CSTR( CSTR_LARGE_GOLD_NUGGET ), 0, R_NUGGET); 
    objs(GOLD).desc[0] = GET_CSTR( CSTR_THERE_IS_LARGE_SPARKLING_NUGGET_OF1 ); 
    new_obj(DIAMONDS, GET_CSTR( CSTR_SEVERAL_DIAMONDS ), 0, R_WFISS); 
    objs(DIAMONDS).desc[0] = GET_CSTR( CSTR_THERE_ARE_DIAMONDS_HERE1 ); 
    new_obj(SILVER, GET_CSTR( CSTR_BARS_OF_SILVER ), 0, R_NS); 
    objs(SILVER).desc[0] = GET_CSTR( CSTR_THERE_ARE_BARS_OF_SILVER_HERE1 ); 
    new_obj(JEWELS, GET_CSTR( CSTR_PRECIOUS_JEWELRY ), 0, R_SOUTH); 
    objs(JEWELS).desc[0] = GET_CSTR( CSTR_THERE_IS_PRECIOUS_JEWELRY_HERE1 ); 
    new_obj(COINS, GET_CSTR( CSTR_RARE_COINS ), 0, R_WEST); 
    objs(COINS).desc[0] = GET_CSTR( CSTR_THERE_ARE_MANY_COINS_HERE1 ); 
    new_obj(CHEST, GET_CSTR( CSTR_TREASURE_CHEST ), 0, R_LIMBO); 
    objs(CHEST).desc[0] = GET_CSTR( CSTR_THE_PIRATE_TREASURE_CHEST_IS_HERE1 ); 
    new_obj(EGGS, GET_CSTR( CSTR_GOLDEN_EGGS ), 0, R_GIANT); 
    objs(EGGS).desc[0] = GET_CSTR( CSTR_THERE_IS_LARGE_NEST_HERE_FULL_OF_GOLDEN2 ); 
    new_obj(TRIDENT, GET_CSTR( CSTR_JEWELED_TRIDENT ), 0, R_FALLS); 
    objs(TRIDENT).desc[0] = GET_CSTR( CSTR_THERE_IS_JEWEL_ENCRUSTED_TRIDENT_HERE1 ); 
    new_obj(VASE, GET_CSTR( CSTR_MING_VASE ), 0, R_ORIENTAL); 
    objs(VASE).desc[0] = GET_CSTR( CSTR_THERE_IS_DELICATE_PRECIOUS_MING_VASE1 ); 
    objs(VASE).desc[1] = GET_CSTR( CSTR_THE_FLOOR_IS_LITTERED_WITH_WORTHLESS1 ); 
    new_obj(EMERALD, GET_CSTR( CSTR_EGG_SIZED_EMERALD ), 0, R_PLOVER); 
    objs(EMERALD).desc[0] = GET_CSTR( CSTR_THERE_IS_AN_EMERALD_HERE_THE_SIZE_OF1 ); 
    new_obj(PYRAMID, GET_CSTR( CSTR_PLATINUM_PYRAMID ), 0, R_DARK); 
    objs(PYRAMID).desc[0] = GET_CSTR( CSTR_THERE_IS_PLATINUM_PYRAMID_HERE_INCHES1 ); 
    new_obj(PEARL, GET_CSTR( CSTR_GLISTENING_PEARL ), 0, R_LIMBO); 
    objs(PEARL).desc[0] = GET_CSTR( CSTR_OFF_TO_ONE_SIDE_LIES_GLISTENING_PEARL1 ); 
    new_obj(RUG_, 0, RUG, R_SCAN3); 
    new_obj(RUG, GET_CSTR( CSTR_PERSIAN_RUG ), RUG, R_SCAN1); 
    objs(RUG).desc[0] = GET_CSTR( CSTR_THERE_IS_PERSIAN_RUG_SPREAD_OUT_ON_THE1 ); 
    objs(RUG).desc[1] = GET_CSTR( CSTR_THE_DRAGON_IS_SPRAWLED_OUT_ON_PERSIAN1 ); 
    new_obj(SPICES, GET_CSTR( CSTR_RARE_SPICES ), 0, R_CHAMBER); 
    objs(SPICES).desc[0] = GET_CSTR( CSTR_THERE_ARE_RARE_SPICES_HERE1 ); 
    new_obj(CHAIN, GET_CSTR( CSTR_GOLDEN_CHAIN ), CHAIN, R_BARR); 
    objs(CHAIN).desc[0] = GET_CSTR( CSTR_THERE_IS_GOLDEN_CHAIN_LYING_IN_HEAP_ON1 ); 
    objs(CHAIN).desc[1] = GET_CSTR( CSTR_THE_BEAR_IS_LOCKED_TO_THE_WALL_WITH1 ); 
    objs(CHAIN).desc[2] = GET_CSTR( CSTR_THERE_IS_GOLDEN_CHAIN_LOCKED_TO_THE1 ); 
} 
 
typedef const char *CTEXT; 
 
// Print a CTEXT without a carriage return 
 
void puts_nocr( CTEXT s ) { 
     
    puts( s ); 
     
}     
 
 
// Print a CTEXT with an embedded int (% is replaced) 
 
void puts_n( CTEXT s , int d ) { 
     
    puts( s ); 
     
} 
 
 
/*========== Input routines. ============================================== 
 * This section corresponds to sections 71--73 in Knuth. 
 */ 
 
#define BUF_SIZE 72 
char buffer[BUF_SIZE]; /* your input goes here */ 
char word1[BUF_SIZE], word2[BUF_SIZE]; /* and then we snarf it to here */ 
 
bool yes( CTEXT q, const char *y, const char *n) 
{ 
    while (true) { 
        puts_nocr( q );  
        puts("\n** "); 
         
        fgets(buffer, sizeof(buffer), stdin); 
        if (tolower(*buffer) == 'y') { 
            if (y) puts(y); 
            return true; 
        } else if (tolower(*buffer) == 'n') { 
            if (n) puts(n); 
            return false; 
        } else { 
            puts(GET_CSTR( CSTR_PLEASE_ANSWER_YES_OR_NO1 )); 
        } 
    } 
} 
 
void listen(void) 
{ 
    char *p, *q; 
    while (true) { 
        puts_nocr("* "); fflush(stdout); 
        fgets(buffer, sizeof(buffer), stdin); 
        for (p = buffer; isspace(*p); ++p) ; 
        if (*p == '\0') { 
            puts(GET_CSTR( CSTR_TELL_ME_TO_DO_SOMETHING1 )); continue; 
        } 
        /* Notice that this algorithm depends on the buffer's being 
         * terminated by "\n\0", or at least some whitespace character. */ 
        for (q = word1; !isspace(*p); ++p, ++q) { 
            *q = tolower(*p); 
        } 
        *q = '\0'; 
        for (++p; isspace(*p); ++p) ; 
        if (*p == '\0') { 
            *word2 = '\0'; return; 
        } 
        for (q = word2; !isspace(*p); ++p, ++q) { 
            *q = tolower(*p); 
        } 
        *q = '\0'; 
        for (++p; isspace(*p); ++p) ; 
        if (*p == '\0') return; 
        puts(GET_CSTR( CSTR_PLEASE_STICK_TO_AND_WORD_COMMANDS1 )); 
    } 
} 
 
void shift_words(void) 
{ 
    strcpy(word1, word2); 
    *word2 = '\0'; 
} 
 
/*========== Dwarves and pirate. ========================================== 
 * This section corresponds to sections 159--175 in Knuth. 
 */ 
 
int dflag;  /* how angry are the dwarves? */ 
 
struct Dwarf { 
    bool seen; 
    Location oldloc; 
    Location loc; 
} dwarves[6] = { 
    { false, R_LIMBO, R_PIRATES_NEST },  /* this one is really the pirate */ 
    { false, R_LIMBO, R_HMK }, 
    { false, R_LIMBO, R_WFISS }, 
    { false, R_LIMBO, R_Y2 }, 
    { false, R_LIMBO, R_LIKE3 }, 
    { false, R_LIMBO, R_COMPLEX }, 
}; 
struct Dwarf *pirate = &dwarves[0]; 
 
bool dwarf_at(Location loc)  /* is a dwarf present? Section 160 in Knuth. */ 
{ 
    if (dflag < 2) return false; 
    for (int j=1; j <= 5; ++j) { 
        if (loc == dwarves[j].loc) return true; 
    } 
    return false; 
} 
 
void return_pirate_to_lair(bool with_chest) 
{ 
    if (with_chest) { 
        drop(CHEST, R_PIRATES_NEST); 
        drop(MESSAGE, R_PONY); 
    } 
    pirate->loc = pirate->oldloc = R_PIRATES_NEST; 
    pirate->seen = false; 
} 
 
bool too_easy_to_steal(ObjectWord t, Location loc) 
{ 
    return (t == PYRAMID && (loc == R_PLOVER || loc == R_DARK)); 
} 
 
void steal_all_your_treasure(Location loc)  /* sections 173--174 in Knuth */ 
{ 
    puts(GET_CSTR( CSTR_OUT_FROM_THE_SHADOWS_BEHIND_YOU1 ) SOFT_NL 
         GET_CSTR( CSTR_HE_CHORTLES_LL_JUST_TAKE_ALL_THIS_BOOTY1 ) SOFT_NL 
         GET_CSTR( CSTR_CHEST_DEEP_IN_THE_MAZE_HE_SNATCHES_YOUR1 ) SOFT_NL 
         GET_CSTR( CSTR_THE_GLOOM )); 
    for (int t = MIN_OBJ; t <= MAX_OBJ; ++t) { 
        if (!is_treasure(t)) continue; 
        if (too_easy_to_steal(t, loc)) continue; 
        if (here(t, loc) && !is_immobile(t)) { 
            /* The vase, rug, and chain can all be immobile at times. */ 
            move(t, R_PIRATES_NEST); 
        } 
    } 
} 
 
void pirate_tracks_you(Location loc) 
{ 
    bool chest_needs_placing = there(MESSAGE, R_LIMBO); 
    bool stalking = false; 
    /* The pirate leaves you alone once you've found the chest. */ 
    if (loc == R_PIRATES_NEST || objs(CHEST).prop >= 0) return; 
    for (int i = MIN_OBJ; i <= MAX_OBJ; ++i) { 
        if (!is_treasure(i)) continue; 
        if (too_easy_to_steal(i, loc)) continue; 
        if (toting(i)) { 
            steal_all_your_treasure(loc); 
            return_pirate_to_lair(chest_needs_placing); 
            return; 
        } 
        if (there(i, loc)) { 
            /* There is a treasure in this room, but we're not carrying 
             * it. The pirate won't pounce unless we're carrying the 
             * treasure; so he'll try to remain quiet. */ 
            stalking = true; 
        } 
    } 
    /* tally is the number of treasures we haven't seen; lost_treasures is 
     * the number we never will see (due to killing the bird or destroying 
     * the troll bridge). */ 
    if (tally == lost_treasures+1 && !stalking && chest_needs_placing && 
        objs(LAMP).prop && here(LAMP, loc)) { 
        /* As soon as we've seen all the treasures (except the ones that are 
         * lost forever), we "cheat" and let the pirate be spotted. Of course 
         * there have to be shadows to hide in, so check the lamp. */ 
        puts(GET_CSTR( CSTR_THERE_ARE_FAINT_RUSTLING_NOISES_FROM3 ) SOFT_NL 
             GET_CSTR( CSTR_TURN_TOWARD_THEM_THE_BEAM_OF_YOUR_LAMP1 ) SOFT_NL 
             GET_CSTR( CSTR_HE_IS_CARRYING_LARGE_CHEST_SHIVER_ME1 ) SOFT_NL 
             GET_CSTR( CSTR_BEEN_SPOTTED_BEST_HIE_MESELF_OFF_TO_THE1 ) SOFT_NL 
             GET_CSTR( CSTR_WITH_THAT_HE_VANISHES_INTO_THE_GLOOM1 )); 
        return_pirate_to_lair(true); 
        return; 
    } 
    if (pirate->oldloc != pirate->loc && pct(20)) { 
        puts(GET_CSTR( CSTR_THERE_ARE_FAINT_RUSTLING_NOISES_FROM2 )); 
    } 
} 
 
bool forbidden_to_pirate(Location loc) 
{ 
    return (loc > R_PIRATES_NEST); 
} 
 
/* Return true if the player got killed by a dwarf this turn. 
 * This function represents sections 161--168, 170--175 in Knuth. */ 
bool move_dwarves_and_pirate(Location loc) 
{ 
    assert(R_LIMBO <= loc && loc <= MAX_LOC); 
    if (forbidden_to_pirate(loc) || loc == R_LIMBO) { 
        /* Bypass all dwarf motion if you are in a place forbidden to the 
         * pirate, or if your next motion is forced. Besides the cases that 
         * Knuth mentions (dwarves can't meet the bear, dwarves can't enter 
         * most dead ends), this also prevents the axe-toting dwarf from 
         * showing up in the middle of a forced move and dropping the axe 
         * in an inaccessible pseudo-location. */ 
    } else if (dflag == 0) { 
        if (loc >= MIN_LOWER_LOC) dflag = 1; 
    } else if (dflag == 1) { 
        if (loc >= MIN_LOWER_LOC && pct(5)) { 
            /* When level 2 of the cave is reached, we silently kill 0, 1, 
             * or 2 of the dwarves. Then if any of the survivors is in 
             * the current location, we move him to R_NUGGET; thus no 
             * dwarf is presently tracking you. Another dwarf does, 
             * however, toss an axe and grumpily leave the scene. */ 
            dflag = 2; 
            if (pct(50)) dwarves[1+ran(5)].loc = R_LIMBO; 
            if (pct(50)) dwarves[1+ran(5)].loc = R_LIMBO; 
            for (int j=1; j <= 5; ++j) { 
                if (dwarves[j].loc == loc) dwarves[j].loc = R_NUGGET; 
                dwarves[j].oldloc = dwarves[j].loc; 
            } 
            /* Knuth quietly fixes the garden-path grammar here: 
             *   A little dwarf just walked around a corner, saw you, threw a 
             *   little axe at you, cursed, and ran away. (The axe missed.) 
             * But Woods' version matches Crowther's original source code, 
             * and I don't think the deviation is justified. */ 
            puts(GET_CSTR( CSTR_LITTLE_DWARF_JUST_WALKED_AROUND1 ) SOFT_NL 
                 GET_CSTR( CSTR_AXE_AT_YOU_WHICH_MISSED_CURSED_AND_RAN1 )); 
            drop(AXE, loc); 
        } 
    } else { 
        /* Move dwarves and the pirate. */ 
        int dtotal = 0;  /* this many dwarves are in the room with you */ 
        int attack = 0;  /* this many have had time to draw their knives */ 
        int stick = 0;  /* this many have hurled their knives accurately */ 
        for (int j=0; j <= 5; ++j) { 
            struct Dwarf *d = &dwarves[j]; 
            if (d->loc != R_LIMBO) { 
                Location ploc[19];  /* potential locations for the next random step */ 
                int i = 0; 
                /* Make a table of all potential exits. 
                 * Dwarves think R_SCAN1, R_SCAN2, R_SCAN3 are three different locations, 
                 * although you will never have that perception. */ 
                for (Instruction *q = start[d->loc]; q < start[d->loc + 1]; ++q) { 
                    Location newloc = q->dest; 
                    if (i != 0 && newloc == ploc[i-1]) continue; 
                    if (newloc < MIN_LOWER_LOC) continue;  /* don't wander above level 2 */ 
                    if (newloc == d->oldloc || newloc == d->loc) continue;  /* don't double back */ 
                    if (q->cond == 100) continue; 
                    if (d == pirate && forbidden_to_pirate(newloc)) continue; 
                    if (is_forced(newloc) || newloc > MAX_LOC) continue; 
                    ploc[i++] = newloc; 
                } 
                if (i==0) ploc[i++] = d->oldloc; 
                d->oldloc = d->loc; 
                d->loc = ploc[ran(i)];  /* this is the random walk */ 
 
                /* Dwarves follow the player once they've spotted him. But 
                 * they won't follow outside the lower cave. */ 
                if (d->loc == loc || d->oldloc == loc) { 
                    d->seen = true; 
                } else if (loc < MIN_LOWER_LOC) { 
                    d->seen = false; 
                } 
 
                if (d->seen) { 
                    /* Make dwarf d follow */ 
                    d->loc = loc; 
                    if (d == pirate) { 
                        pirate_tracks_you(loc); 
                    } else { 
                        ++dtotal; 
                        if (d->oldloc == d->loc) { 
                            ++attack; 
                            last_knife_loc = loc; 
                            if (ran(1000) < 95*(dflag-2)) ++stick; 
                        } 
                    } 
                } 
            } 
        } 
        if (dtotal != 0) { 
            /* Make the threatening dwarves attack. */ 
            if (dtotal == 1) { 
                puts(GET_CSTR( CSTR_THERE_IS_THREATENING_LITTLE_DWARF_IN1 )); 
            } else { 
                puts_n(GET_CSTR( CSTR_THERE_ARE_THREATENING_LITTLE_DWARVES1 ), dtotal); 
            } 
            if (attack) { 
                if (dflag == 2) dflag = 3; 
                if (attack == 1) { 
                    puts(GET_CSTR( CSTR_ONE_SHARP_NASTY_KNIFE_IS_THROWN_AT_YOU1 )); 
                    if (stick == 0) puts(GET_CSTR( CSTR_IT_MISSES )); 
                    else puts(GET_CSTR( CSTR_IT_GETS_YOU )); 
                } else { 
                    puts_n(GET_CSTR( CSTR_OF_THEM_THROW_KNIVES_AT_YOU1 ), attack); 
                    if (stick == 0) puts(GET_CSTR( CSTR_NONE_OF_THEM_HIT_YOU1 )); 
                    else if (stick == 1) puts(GET_CSTR( CSTR_ONE_OF_THEM_GETS_YOU1 )); 
                    else puts_n(GET_CSTR( CSTR_OF_THEM_GET_YOU ), stick); 
                } 
                if (stick) return true;  /* goto death */ 
            } 
        } 
    } 
    return false;  /* the player survived this function */ 
} 
 
 
/*========== Closing the cave. ============================================ 
 * This section corresponds to sections 103, 178--181 in Knuth. 
 */ 
 
int lamp_limit;  /* countdown till darkness */ 
int clock1 = 15, clock2 = 30;  /* clocks that govern closing time */ 
bool closed;  /* set only when you're in the repository */ 
int bonus;  /* extra points awarded for exceptional adventuring skills */ 
 
bool cave_is_closing(void) 
{ 
    return (clock1 < 0); 
} 
 
void close_the_cave(void) 
{ 
    /* Woods has "entones", but that's not a word, so I'm changing it. 
     * Knuth writes "Then your eyes refocus;" in place of "As your eyes 
     * refocus," but I don't see any reason to deviate from Woods' 
     * wording there. Maybe Knuth was working from a slightly earlier 
     * or later version than the one I'm looking at. */ 
    puts(GET_CSTR( CSTR_THE_SEPULCHRAL_VOICE_INTONES_THE_CAVE1 ) SOFT_NL 
         GET_CSTR( CSTR_FADE_THERE_IS_BLINDING_FLASH_OF_LIGHT1 ) SOFT_NL 
         GET_CSTR( CSTR_SMOKE_AS_YOUR_EYES_REFOCUS_YOU_LOOK1 )); 
    move(BOTTLE, R_NEEND); objs(BOTTLE).prop = -2;  /* empty */ 
    move(PLANT, R_NEEND); objs(PLANT).prop = -1; 
    move(OYSTER, R_NEEND); objs(OYSTER).prop = -1; 
    move(LAMP, R_NEEND); objs(LAMP).prop = -1; 
    move(ROD, R_NEEND); objs(ROD).prop = -1; 
    move(DWARF, R_NEEND); objs(DWARF).prop = -1; 
    move(MIRROR, R_NEEND); objs(MIRROR).prop = -1; 
    move(GRATE, R_SWEND); objs(GRATE).prop = 0; 
    move(SNAKE, R_SWEND); objs(SNAKE).prop = -2;  /* not blocking the way */ 
    move(BIRD, R_SWEND); objs(BIRD).prop = -2;  /* caged */ 
    move(CAGE, R_SWEND); objs(CAGE).prop = -1; 
    move(ROD2, R_SWEND); objs(ROD2).prop = -1; 
    move(PILLOW, R_SWEND); objs(PILLOW).prop = -1; 
    move(MIRROR_, R_SWEND); 
    for (int j = MIN_OBJ; j <= MAX_OBJ; ++j) { 
        if (toting(j)) destroy(j); 
    } 
    closed = true; 
    bonus = 10; 
} 
 
/* Return true if the cave just closed. */ 
bool check_clocks_and_lamp(Location loc) 
{ 
    if (tally == 0 && loc >= MIN_LOWER_LOC && loc != R_Y2) 
        --clock1; 
    if (clock1 == 0) { 
        /* At the time of first warning, we lock the grate, destroy the 
         * crystal bridge, kill all the dwarves (and the pirate), and 
         * remove the troll and bear (unless dead). 
         * It's too much trouble to move the dragon, so we leave it. 
         * From now on until clock2 runs out, you cannot unlock the grate, 
         * move to any location outside the cave, or create the bridge. 
         * Nor can you be resurrected if you die. */ 
        puts(GET_CSTR( CSTR_SEPULCHRAL_VOICE_REVERBERATING1 ) SOFT_NL 
             GET_CSTR( CSTR_CLOSING_SOON_ALL_ADVENTURERS_EXIT1 )); 
        clock1 = -1; 
        objs(GRATE).prop = 0; 
        objs(FISSURE).prop = 0; 
        for (int j=0; j <= 5; ++j) { 
            dwarves[j].seen = false; 
            dwarves[j].loc = R_LIMBO; 
        } 
        destroy(TROLL); destroy(TROLL_); 
        move(NO_TROLL, R_SWSIDE); move(NO_TROLL_, R_NESIDE); 
        juggle(CHASM); juggle(CHASM_); 
        if (objs(BEAR).prop != 3) destroy(BEAR); 
        objs(CHAIN).prop = 0; mobilize(CHAIN); 
        objs(AXE).prop = 0; mobilize(AXE); 
    } else { 
        if (cave_is_closing()) --clock2; 
        if (clock2 == 0) { 
            close_the_cave(); 
            return true; 
        } else { 
            static bool warned = false; 
            /* On every turn (if the cave is not closed), we check to see 
             * if you are in trouble lampwise. */ 
            if (objs(LAMP).prop == 1) --lamp_limit; 
            if (lamp_limit <= 30 && here(LAMP, loc) && here(BATTERIES, loc) && objs(BATTERIES).prop == 0) { 
                puts(GET_CSTR( CSTR_YOUR_LAMP_IS_GETTING_DIM_TAKING_THE1 ) SOFT_NL 
                     GET_CSTR( CSTR_THE_BATTERIES )); 
                objs(BATTERIES).prop = 1; 
                if (toting(BATTERIES)) drop(BATTERIES, loc); 
                lamp_limit = 2500; 
            } else if (lamp_limit == 0) { 
                if (here(LAMP, loc)) puts(GET_CSTR( CSTR_YOUR_LAMP_HAS_RUN_OUT_OF_POWER2 )); 
                objs(LAMP).prop = 0; 
                lamp_limit = -1; 
            } else if (lamp_limit < 0 && loc < MIN_IN_CAVE) { 
                puts(GET_CSTR( CSTR_THERE_NOT_MUCH_POINT_IN_WANDERING1 ) SOFT_NL 
                     GET_CSTR( CSTR_EXPLORE_THE_CAVE_WITHOUT_LAMP_SO_LET1 )); 
                give_up(); 
            } else if (lamp_limit < 30 && !warned && here(LAMP, loc)) { 
                puts(GET_CSTR( CSTR_YOUR_LAMP_IS_GETTING_DIM1 )); 
                if (objs(BATTERIES).prop == 1) { 
                    puts(GET_CSTR( CSTR_AND_YOU_RE_OUT_OF_SPARE_BATTERIES_YOU1 ) SOFT_NL 
                         GET_CSTR( CSTR_BEST_START_WRAPPING_THIS_UP1 )); 
                } else if (there(BATTERIES, R_LIMBO)) { 
                    puts(GET_CSTR( CSTR_YOU_BEST_START_WRAPPING_THIS_UP_UNLESS1 ) SOFT_NL 
                         GET_CSTR( CSTR_YOU_CAN_FIND_SOME_FRESH_BATTERIES_SEEM1 ) SOFT_NL 
                         GET_CSTR( CSTR_VENDING_MACHINE_IN_THE_MAZE_BRING_SOME1 )); 
                } else { 
                    puts(GET_CSTR( CSTR_YOU_BEST_GO_BACK_FOR_THOSE_BATTERIES1 )); 
                } 
                warned = true; 
            } 
        } 
    } 
    return false; 
} 
 
void panic_at_closing_time(void) 
{ 
    /* If you try to get out while the cave is closing, we assume that 
     * you panic; we give you a few additional turns to get frantic 
     * before we close. */ 
    static bool panic = false; 
    if (!panic) { 
        clock2 = 15; 
        panic = true; 
    } 
    puts(GET_CSTR( CSTR_MYSTERIOUS_RECORDED_VOICE_GROANS1 ) SOFT_NL 
         GET_CSTR( CSTR_THIS_EXIT_IS_CLOSED_PLEASE_LEAVE_VIA1 )); 
    /* Woods does NOT set "was_dark = false" at this point. 
     * This means that if you've gotten into the habit of turning 
     * off your lamp before you use a magic word to teleport out 
     * of the cave, we might well add injury to insult by causing 
     * you to fall into a pit and die (thus ending the game) right 
     * at this point. */ 
} 
 
 
 
/*========== The main loop. =============================================== 
 * This section corresponds to sections 74--75 in Knuth. 
 */ 
 
int turns;  /* how many times we've read your commands */ 
int verbose_interval = 5;  /* command BRIEF sets this to 10000 */ 
int foobar;  /* progress in the FEE FIE FOE FOO incantation */ 
 
void give_optional_plugh_hint(Location loc) 
{ 
    if (loc == R_Y2 && pct(25) && !cave_is_closing()) { 
        puts(GET_CSTR( CSTR_HOLLOW_VOICE_SAYS_PLUGH1 )); 
    } 
} 
 
void spot_treasure(ObjectWord t) 
{ 
    if (objs(t).prop >= 0) return; 
    assert(is_treasure(t) && !closed);  /* You've spotted a treasure */ 
    switch (t) { 
        case RUG:  /* trapped */ 
        case CHAIN:  /* locked */ 
            objs(t).prop = 1; 
            break; 
        default: 
            objs(t).prop = 0; 
            break; 
    } 
    tally--; 
    if (tally == lost_treasures && tally > 0 && lamp_limit > 35) { 
        /* Zap the lamp if the remaining treasures are too elusive */ 
        lamp_limit = 35; 
    } 
} 
 
void describe_object(ObjectWord t, Location loc) 
{ 
    if (t == TREADS && toting(GOLD)) { 
        /* The rough stone steps disappear if we are carrying the nugget. */ 
        return; 
    } 
    int going_up = (t == TREADS && loc == R_EMIST); 
    const char *obj_description = objs(t).desc[objs(t).prop + going_up]; 
    if (obj_description != NULL) { 
        puts(obj_description); 
    } 
} 
 
int look_around(Location loc, bool dark, bool was_dark) 
{ 
     CTEXT room_description; 
    if (dark && !is_forced(loc)) { 
        if (was_dark && pct(35)) return 'p';  /* goto pitch_dark; */ 
        room_description = pitch_dark_msg; 
    } else if (places[loc].short_desc == NULL || places[loc].visits % verbose_interval == 0) { 
        room_description = places[loc].long_desc; 
    } else { 
        room_description = places[loc].short_desc; 
    } 
    if (toting(BEAR)) { 
        puts(GET_CSTR( CSTR_YOU_ARE_BEING_FOLLOWED_BY_VERY_LARGE2 )); 
    } 
    if (room_description != NULL) { 
        /* R_CHECK's description is NULL. */ 
        puts( room_description ); 
    } 
    if (is_forced(loc)) return 't';  /* goto try_move; */ 
    give_optional_plugh_hint(loc); 
    if (!dark) { 
        places[loc].visits += 1; 
        /* Describe the objects at this location. */ 
        for (ObjectWord t = places[loc].objects; t != NOTHING; t = objs(t).link) { 
            ObjectWord tt = objs(t).base ? objs(t).base : t; 
            if (closed && (objs(tt).prop < 0)) continue;  /* scenery objects */ 
            spot_treasure(tt); 
            describe_object(tt, loc); 
        } 
    } 
    return 0;  /* just continue normally */ 
} 
 
 
/*========== Hints. ======================================================= 
 * This section corresponds to sections 80 and 193--196 in Knuth. 
 */ 
 
void advise_about_going_west(const char *word1) 
{ 
    /* Here's a freely offered hint that may save you typing. */ 
    static int west_count = 0; 
    if (streq(word1, GET_CSTR( CSTR_WEST ))) { 
        ++west_count; 
        if (west_count == 10) { 
            puts(GET_CSTR( CSTR_IF_YOU_PREFER_SIMPLY_TYPE_RATHER_THAN1 )); 
        } 
    } 
} 
 
struct { 
    int count; 
    bool given; 
    int thresh; 
    int cost; 
    const char *prompt; 
    const char *hint; 
} hints[] = { 
    { 0, false, 0, 5, GET_CSTR( CSTR_WELCOME_TO_ADVENTURE_WOULD_YOU_LIKE1 ), 
    GET_CSTR( CSTR_SOMEWHERE_NEARBY_IS_COLOSSAL_CAVE1 ) SOFT_NL 
    GET_CSTR( CSTR_TREASURE_AND_GOLD_THOUGH_IT_IS_RUMORED1 ) SOFT_NL 
    GET_CSTR( CSTR_SEEN_AGAIN_MAGIC_IS_SAID_TO_WORK_IN_THE1 ) SOFT_NL 
    GET_CSTR( CSTR_AND_HANDS_DIRECT_ME_WITH_COMMANDS_OF_OR1 ) SOFT_NL 
    GET_CSTR( CSTR_YOU_THAT_LOOK_AT_ONLY_THE_FIRST_FIVE1 ) SOFT_NL 
    GET_CSTR( CSTR_HAVE_TO_ENTER_NORTHEAST_AS_NE_TO1 ) SOFT_NL 
    GET_CSTR( CSTR_SHOULD_YOU_GET_STUCK_TYPE_HELP_FOR_SOME1 ) SOFT_HYPHEN 
    GET_CSTR( CSTR_MATION_ON_HOW_TO_END_YOUR_ADVENTURE_ETC1 ) SOFT_NL 
    GET_CSTR( CSTR_DON_KNUTH_AND_THEN_FROM_CWEB_TO_ANSI_BY1 ) }, 
    { 0, false, 0, 10, 
    GET_CSTR( CSTR_HMMM_THIS_LOOKS_LIKE_CLUE_WHICH_MEANS1 ) SOFT_NL 
    GET_CSTR( CSTR_READ_IT_SHOULD_GO_AHEAD_AND_READ_IT1 ), 
    GET_CSTR( CSTR_IT_SAYS_THERE_IS_SOMETHING_STRANGE1 ) SOFT_NL 
    GET_CSTR( CSTR_OF_THE_WORDS_VE_ALWAYS_KNOWN_NOW_HAS_NEW1 ) }, 
    { 0, false, 4, 2, GET_CSTR( CSTR_ARE_YOU_TRYING_TO_GET_INTO_THE_CAVE1 ), 
    GET_CSTR( CSTR_THE_GRATE_IS_VERY_SOLID_AND_HAS1 ) SOFT_NL 
    GET_CSTR( CSTR_ENTER_WITHOUT_KEY_AND_THERE_ARE_NO_KEYS1 ) SOFT_NL 
    GET_CSTR( CSTR_LOOKING_ELSEWHERE_FOR_THE_KEYS1 ) }, 
    { 0, false, 5, 2, GET_CSTR( CSTR_ARE_YOU_TRYING_TO_CATCH_THE_BIRD1 ), 
    GET_CSTR( CSTR_SOMETHING_SEEMS_TO_BE_FRIGHTENING_THE1 ) SOFT_NL 
    GET_CSTR( CSTR_CATCH_IT_NO_MATTER_WHAT_YOU_TRY_PERHAPS1 ) }, 
    { 0, false, 8, 2, GET_CSTR( CSTR_ARE_YOU_TRYING_TO_DEAL_SOMEHOW_WITH_THE1 ), 
    GET_CSTR( CSTR_YOU_CAN_KILL_THE_SNAKE_OR_DRIVE_IT_AWAY1 ) SOFT_NL 
    GET_CSTR( CSTR_LIKE_THAT_THERE_IS_WAY_TO_GET_BY_BUT_YOU1 ) SOFT_NL 
    GET_CSTR( CSTR_RESOURCES_RIGHT_NOW ) }, 
    { 0, false, 75, 4, GET_CSTR( CSTR_DO_YOU_NEED_HELP_GETTING_OUT_OF_THE_MAZE1 ), 
    GET_CSTR( CSTR_YOU_CAN_MAKE_THE_PASSAGES_LOOK_LESS1 ) }, 
    { 0, false, 25, 5, GET_CSTR( CSTR_ARE_YOU_TRYING_TO_EXPLORE_BEYOND_THE1 ), 
    GET_CSTR( CSTR_THERE_IS_WAY_TO_EXPLORE_THAT_REGION1 ) SOFT_NL 
    GET_CSTR( CSTR_FALLING_INTO_PIT_NONE_OF_THE_OBJECTS1 ) SOFT_NL 
    GET_CSTR( CSTR_USEFUL_IN_DISCOVERING_THE_SECRET1 ) }, 
    { 0, false, 20, 3, GET_CSTR( CSTR_DO_YOU_NEED_HELP_GETTING_OUT_OF_HERE1 ), 
    GET_CSTR( CSTR_DON_GO_WEST ) } 
}; 
 
void offer(int j) 
{ 
    if (j > 1) { 
        if (!yes(hints[j].prompt, GET_CSTR( CSTR_AM_PREPARED_TO_GIVE_YOU_HINT1 ), ok)) return; 
        puts_n(GET_CSTR( CSTR_BUT_IT_WILL_COST_YOU_POINTS1 ), hints[j].cost); 
        hints[j].given = yes(GET_CSTR( CSTR_DO_YOU_WANT_THE_HINT1 ), hints[j].hint, ok); 
    } else { 
        hints[j].given = yes(hints[j].prompt, hints[j].hint, ok); 
    } 
    if (hints[j].given && lamp_limit > 30) { 
        /* Give the lamp some more power. */ 
        lamp_limit += 30*hints[j].cost; 
    } 
} 
 
void maybe_give_a_hint(Location loc, Location oldloc, Location oldoldloc, ObjectWord oldobj) 
{ 
    /* Check if a hint applies, and give it if requested. */ 
    unsigned int k = F_CAVE_HINT; 
    for (int j = 2; j <= 7; ++j, k <<= 1) { 
        if (hints[j].given) continue; 
        if ((places[loc].flags & k) == 0) { 
            /* We've left the map region associated with this hint. */ 
            hints[j].count = 0; 
            continue; 
        } 
        /* Count the number of turns spent here. */ 
        if (++hints[j].count >= hints[j].thresh) { 
            switch (j) { 
                case 2:  /* How to get into the cave. */ 
                    if (!objs(GRATE).prop && !here(KEYS, loc)) { 
                        offer(j); 
                    } 
                    hints[j].count = 0; 
                    break; 
                case 3:  /* Are you trying to catch the bird? */ 
                    /* Notice that this hint is offered even when the bird has 
                     * already been caught, if you spend enough time in the bird 
                     * chamber carrying the rod and experimenting with the bird. 
                     * This behavior is in Woods' Fortran original. */ 
                    if (here(BIRD, loc) && oldobj == BIRD && toting(ROD)) { 
                        offer(j); 
                        hints[j].count = 0; 
                    } 
                    break; 
                case 4:  /* How to deal with the snake. */ 
                    if (here(SNAKE, loc) && !here(BIRD, loc)) { 
                        offer(j); 
                    } 
                    hints[j].count = 0; 
                    break; 
                case 5:  /* How to map the twisty passages all alike. */ 
                    if (places[loc].objects == NOTHING && 
                            places[oldloc].objects == NOTHING && 
                            places[oldoldloc].objects == NOTHING && 
                            holding_count > 1) { 
                        offer(j); 
                    } 
                    hints[j].count = 0; 
                    break; 
                case 6:  /* How to explore beyond the Plover Room. */ 
                    if (objs(EMERALD).prop != -1 && objs(PYRAMID).prop == -1) { 
                        offer(j); 
                    } 
                    hints[j].count = 0; 
                    break; 
                case 7:  /* How to get out of Witt's End. */ 
                    offer(j); 
                    hints[j].count = 0; 
                    break; 
            } 
        } 
    } 
} 
 
/*========== Scoring. ===================================================== 
 * This section corresponds to sections 193 and 197 in Knuth. 
 */ 
 
#define MAX_SCORE 350 
#define MAX_DEATHS 3 
bool gave_up; 
int death_count; 
 
int score(void) 
{ 
    int s = 2; 
    if (dflag != 0) s += 25; 
    for (int i = MIN_OBJ; i <= MAX_OBJ; ++i) { 
        if (!is_treasure(i)) continue; 
        if (objs(i).prop >= 0) { 
            s += 2;  /* two points just for seeing a treasure */ 
            if (there(i, R_HOUSE) && objs(i).prop == 0) { 
                if (i < CHEST) { 
                    s += 10; 
                } else if (i == CHEST) { 
                    s += 12; 
                } else { 
                    s += 14; 
                } 
            } 
        } 
    } 
    s += 10 * (MAX_DEATHS - death_count); 
    if (!gave_up) s += 4; 
    if (there(MAG, R_WITT)) s += 1;  /* last lousy point */ 
    if (cave_is_closing()) s += 25;  /* bonus for making it this far */ 
    s += bonus; 
    for (int i = 0; i < 8; ++i) { 
        if (hints[i].given) s -= hints[i].cost; 
    } 
    return s; 
} 
 
void say_score(void) 
{ 
    puts_n(GET_CSTR( CSTR_IF_YOU_WERE_TO_QUIT_NOW_YOU_WOULD_SCORE1 ) , score()-4); 
    puts_n(GET_CSTR( CSTR_OUT_OF_POSSIBLE ), MAX_SCORE); 
     
    if (yes(GET_CSTR( CSTR_DO_YOU_INDEED_WISH_TO_QUIT_NOW1 ), ok, ok)) give_up(); 
} 
 
#define HIGHEST_CLASS 8 
static int class_score[] = { 35, 100, 130, 200, 250, 300, 330, 349, 9999 }; 
static const char *class_message[] = { 
    GET_CSTR( CSTR_YOU_ARE_OBVIOUSLY_RANK_AMATEUR_BETTER1 ), 
    GET_CSTR( CSTR_YOUR_SCORE_QUALIFIES_YOU_AS_NOVICE1 ), 
    GET_CSTR( CSTR_YOU_HAVE_ACHIEVED_THE_RATING1 ), 
    GET_CSTR( CSTR_YOU_MAY_NOW_CONSIDER_YOURSELF1 ), 
    GET_CSTR( CSTR_YOU_HAVE_REACHED_JUNIOR_MASTER_STATUS1 ), 
    GET_CSTR( CSTR_YOUR_SCORE_PUTS_YOU_IN_MASTER3 ), 
    GET_CSTR( CSTR_YOUR_SCORE_PUTS_YOU_IN_MASTER4 ), 
    GET_CSTR( CSTR_YOUR_SCORE_PUTS_YOU_IN_MASTER5 ), 
    GET_CSTR( CSTR_ALL_OF_ADVENTUREDOM_GIVES_TRIBUTE_TO1 ) 
}; 
 
void give_up(void) 
{ 
    gave_up = true; 
    quit(); 
} 
 
void quit(void) 
{ 
    /* Print the score and say adieu. */ 
    int s = score(); 
    int rank; 
    puts_n(GET_CSTR( CSTR_YOU_SCORED ) , s ); 
    puts_n(GET_CSTR( CSTR_OUT_OF_POSSIBLE1 ) , MAX_SCORE ); 
    puts_n(GET_CSTR( CSTR_USING_TURNS ) , turns);     
    exit(0); 
} 
 
CTEXT death_wishes[2*MAX_DEATHS] = { 
    GET_CSTR( CSTR_OH_DEAR_YOU_SEEM_TO_HAVE_GOTTEN1 ) SOFT_NL 
      GET_CSTR( CSTR_HELP_YOU_OUT_BUT_VE_NEVER_REALLY_DONE1 ) SOFT_NL 
      GET_CSTR( CSTR_TO_TRY_TO_REINCARNATE_YOU1 ), 
    GET_CSTR( CSTR_ALL_RIGHT_BUT_DON_BLAME_ME_IF_SOMETHING1 ) SOFT_NL 
      GET_CSTR( CSTR_YOU_EMERGE_FROM_THE_SMOKE_AND_FIND1 ), 
    GET_CSTR( CSTR_YOU_CLUMSY_OAF_YOU_VE_DONE_IT_AGAIN_DON1 ) SOFT_NL 
      GET_CSTR( CSTR_KEEP_THIS_UP_DO_YOU_WANT_ME_TO_TRY1 ), 
    GET_CSTR( CSTR_OKAY_NOW_WHERE_DID_PUT_MY_ORANGE_SMOKE1 ), 
    GET_CSTR( CSTR_NOW_YOU_VE_REALLY_DONE_IT_OUT_OF_ORANGE1 ) SOFT_NL 
      GET_CSTR( CSTR_ME_TO_DO_DECENT_REINCARNATION_WITHOUT1 ), 
    GET_CSTR( CSTR_OKAY_IF_YOU_RE_SO_SMART_DO_IT_YOURSELF1 ) 
}; 
 
void kill_the_player(Location last_safe_place) 
{ 
    death_count++; 
    if (cave_is_closing()) { 
        puts(GET_CSTR( CSTR_IT_LOOKS_AS_THOUGH_YOU_RE_DEAD_WELL1 )); 
        quit(); 
    } 
    if (!yes(death_wishes[2*death_count-2], death_wishes[2*death_count-1], ok) || death_count == MAX_DEATHS) 
        quit(); 
    /* At this point you are reborn. */ 
    if (toting(LAMP)) objs(LAMP).prop = 0; 
    objs(WATER).place = R_LIMBO; 
    objs(OIL).place = R_LIMBO; 
    for (int j = MAX_OBJ; j >= MIN_OBJ; --j) { 
        if (toting(j)) drop(j, (j == LAMP) ? R_ROAD : last_safe_place); 
    } 
} 
 
/*========== Main loop. =================================================== 
 * This section corresponds to sections 74--158 in Knuth. 
 */ 
 
bool now_in_darkness(Location loc) 
{ 
    if (has_light(loc)) return false; 
    if (here(LAMP, loc) && objs(LAMP).prop) return false; 
    return true; 
} 
 
/* Sections 158, 169, 182 in Knuth */ 
void adjustments_before_listening(Location loc) 
{ 
    if (last_knife_loc != loc) { 
        /* When we move, the dwarf's vanishing knife goes out of scope. */ 
        last_knife_loc = R_LIMBO; 
    } 
    if (closed) { 
        /* After the cave is closed, we look for objects being toted with 
         * prop < 0; their prop value is changed to -1 - prop. This means 
         * they won't be described until they've been picked up and put 
         * down, separate from their respective piles. Section 182 in Knuth. */ 
        if (objs(OYSTER).prop < 0 && toting(OYSTER)) { 
            puts(GET_CSTR( CSTR_INTERESTING_THERE_SEEMS_TO_BE1 ) SOFT_NL 
                 GET_CSTR( CSTR_THE_OYSTER )); 
        } 
        for (int j = MIN_OBJ; j <= MAX_OBJ; ++j) { 
            if (toting(j) && objs(j).prop < 0) 
                objs(j).prop = -1 - objs(j).prop; 
        } 
    } 
} 
 
Location attempt_plover_passage(Location from)  /* section 149 in Knuth */ 
{ 
    if (holding_count == !!toting(EMERALD)) 
        return R_ALCOVE + R_PLOVER - from; 
    puts(GET_CSTR( CSTR_SOMETHING_YOU_RE_CARRYING_WON_FIT1 ) SOFT_NL 
         GET_CSTR( CSTR_YOU_BEST_TAKE_INVENTORY_AND_DROP1 )); 
    return from; 
} 
 
void attempt_inventory(void)  /* section 94 in Knuth */ 
{ 
    bool holding_anything = false; 
    for (ObjectWord t = MIN_OBJ; t <= MAX_OBJ; ++t) { 
        if (toting(t) && t != BEAR) { 
            if (!holding_anything) { 
                holding_anything = true; 
                puts(GET_CSTR( CSTR_YOU_ARE_CURRENTLY_HOLDING_THE1 )); 
            } 
            puts(""); 
            puts( objs(t).name ); 
        } 
    } 
    if (toting(BEAR)) { 
        puts(GET_CSTR( CSTR_YOU_ARE_BEING_FOLLOWED_BY_VERY_LARGE3 )); 
    } else if (!holding_anything) { 
        puts(GET_CSTR( CSTR_YOU_RE_NOT_CARRYING_ANYTHING1 )); 
    } 
} 
 
void attempt_eat(ObjectWord obj)  /* section 98 in Knuth */ 
{ 
    switch (obj) { 
        case FOOD: 
            destroy(FOOD); 
            puts(GET_CSTR( CSTR_THANK_YOU_IT_WAS_DELICIOUS1 )); 
            break; 
        case BIRD: case SNAKE: case CLAM: case OYSTER: 
        case DWARF: case DRAGON: case TROLL: case BEAR: 
            puts(GET_CSTR( CSTR_THINK_JUST_LOST_MY_APPETITE1 )); 
            break; 
        default: 
            puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS )); 
            break; 
    } 
} 
 
void take_something_immobile(ObjectWord obj) 
{ 
    if (obj == CHAIN && objs(BEAR).prop != 0) { 
        puts(GET_CSTR( CSTR_THE_CHAIN_IS_STILL_LOCKED1 )); 
    } else if (obj == BEAR && objs(BEAR).prop == 1) { 
        puts(GET_CSTR( CSTR_THE_BEAR_IS_STILL_CHAINED_TO_THE_WALL1 )); 
    } else if (obj == PLANT && objs(PLANT).prop <= 0) { 
        puts(GET_CSTR( CSTR_THE_PLANT_HAS_EXCEPTIONALLY_DEEP1 )); 
    } else { 
        puts(GET_CSTR( CSTR_YOU_CAN_BE_SERIOUS1 )); 
    } 
} 
 
/* The verb is TAKE. Returns true if the action is finished. */ 
bool take_bird_or_cage(ObjectWord obj) 
{ 
    if (obj == BIRD && !objs(BIRD).prop) { 
        if (toting(ROD)) { 
            puts(GET_CSTR( CSTR_THE_BIRD_WAS_UNAFRAID_WHEN_YOU_ENTERED1 ) SOFT_NL 
                 GET_CSTR( CSTR_DISTURBED_AND_YOU_CANNOT_CATCH_IT1 )); 
            return true; 
        } else if (!toting(CAGE)) { 
            puts(GET_CSTR( CSTR_YOU_CAN_CATCH_THE_BIRD_BUT_YOU_CANNOT1 )); 
            return true; 
        } else { 
            objs(BIRD).prop = 1; 
        } 
    } 
    /* At this point the TAKE action is guaranteed to succeed, so 
     * let's also deal with the cage. Taking a caged bird means also 
     * taking the cage; taking a cage with a bird in it means also 
     * taking the bird. */ 
    if (obj == BIRD) carry(CAGE); 
    if (obj == CAGE && objs(BIRD).prop) carry(BIRD); 
    return false; 
} 
 
/* Return true if the command was TAKE WATER or TAKE OIL, 
 * and there's not a bottle of that substance on the ground. 
 * We'll redirect it to FILL BOTTLE and try again. */ 
bool attempt_take(ObjectWord obj, Location loc) 
{ 
    if (toting(obj)) { 
        puts(GET_CSTR( CSTR_YOU_ARE_ALREADY_CARRYING_IT2 )); 
        return false; 
    } else if (is_immobile(obj)) { 
        take_something_immobile(obj); 
        return false; 
    } else if (obj != NOTHING && here(BOTTLE, loc) && obj == bottle_contents()) { 
        obj = BOTTLE;  /* take the bottle of water or oil */ 
    } else if (obj == WATER || obj == OIL) { 
        if (toting(BOTTLE)) 
            return true;  /* redirect to FILL BOTTLE */ 
        puts(GET_CSTR( CSTR_YOU_HAVE_NOTHING_IN_WHICH_TO_CARRY_IT1 )); 
        return false; 
    } 
    if (holding_count >= 7) { 
        puts(GET_CSTR( CSTR_YOU_CAN_CARRY_ANYTHING_MORE_YOU_LL_HAVE1 )); 
    } else if (take_bird_or_cage(obj)) { 
        /* The bird was uncatchable. */ 
    } else { 
        carry(obj); 
        if (obj == BOTTLE && bottle_contents() != NOTHING) 
            objs(bottle_contents()).place = R_INHAND; 
        puts(ok); 
    } 
    return false; 
} 
 
void attempt_drop(ObjectWord obj, Location loc) 
{ 
    if (obj == ROD && toting(ROD2) && !toting(ROD)) { 
        obj = ROD2; 
    } 
 
    if (!toting(obj)) { 
        puts(GET_CSTR( CSTR_YOU_AREN_CARRYING_IT5 )); 
    } else if (obj == COINS && here(MACHINE, loc)) { 
        /* Put coins in the vending machine. */ 
        destroy(COINS); 
        drop(BATTERIES, loc); 
        objs(BATTERIES).prop = 0; 
        puts(GET_CSTR( CSTR_THERE_ARE_FRESH_BATTERIES_HERE2 )); 
    } else if (obj == VASE && loc != R_SOFT) { 
        drop(VASE, loc); 
        if (there(PILLOW, loc)) { 
            /* In Long's "Adventure 6", the response is this message 
             * plus the default "Dropped." */ 
            puts(GET_CSTR( CSTR_THE_VASE_IS_NOW_RESTING_DELICATELY_ON1 )); 
        } else { 
            puts(GET_CSTR( CSTR_THE_MING_VASE_DROPS_WITH_DELICATE1 )); 
            objs(VASE).prop = 1;  /* the vase is now broken */ 
            immobilize(VASE); 
        } 
    } else if (obj == BEAR && is_at_loc(TROLL, loc)) { 
        /* Chase the troll away. */ 
        puts(GET_CSTR( CSTR_THE_BEAR_LUMBERS_TOWARD_THE_TROLL_WHO1 ) SOFT_NL 
             GET_CSTR( CSTR_SCURRIES_AWAY_THE_BEAR_SOON_GIVES_UP1 )); 
        destroy(TROLL); destroy(TROLL_); 
        drop(NO_TROLL, R_SWSIDE); drop(NO_TROLL_, R_NESIDE); 
        objs(TROLL).prop = 2; 
        juggle(CHASM); juggle(CHASM_);  /* put first in their lists */ 
        drop(BEAR, loc); 
    } else if (obj == BIRD && here(SNAKE, loc)) { 
        puts(GET_CSTR( CSTR_THE_LITTLE_BIRD_ATTACKS_THE_GREEN3 ) SOFT_NL 
             GET_CSTR( CSTR_DRIVES_THE_SNAKE_AWAY1 )); 
        if (closed) dwarves_upset(); 
        drop(BIRD, loc); 
        objs(BIRD).prop = 0;  /* no longer in the cage */ 
        destroy(SNAKE); 
        objs(SNAKE).prop = 1;  /* used in conditional Instructions */ 
    } else if (obj == BIRD && is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) { 
        puts(GET_CSTR( CSTR_THE_LITTLE_BIRD_ATTACKS_THE_GREEN2 ) SOFT_NL 
             GET_CSTR( CSTR_GETS_BURNT_TO_CINDER_THE_ASHES_BLOW1 )); 
        destroy(BIRD); 
        objs(BIRD).prop = 0;  /* no longer in the cage */ 
        /* Now that the bird is dead, you can never get past the snake 
         * into the south side chamber, so the precious jewelry is lost. */ 
        if (there(SNAKE, R_HMK)) ++lost_treasures; 
    } else { 
        /* The usual case for dropping any old object. */ 
 
        if (obj == BIRD) objs(BIRD).prop = 0;  /* no longer caged */ 
        if (obj == CAGE && objs(BIRD).prop) drop(BIRD, loc); 
 
        /* Special cases for dropping a liquid. */ 
        if (obj == WATER && objs(BOTTLE).prop == 0) obj = BOTTLE; 
        if (obj == OIL && objs(BOTTLE).prop == 2) obj = BOTTLE; 
        if (obj == BOTTLE && bottle_contents() != NOTHING) 
            move(bottle_contents(), R_LIMBO); 
 
        drop(obj, loc); 
        puts(ok); 
    } 
} 
 
void attempt_wave(ObjectWord obj, Location loc)  /* section 99 in Knuth */ 
{ 
    if (obj == ROD && (loc == R_EFISS || loc == R_WFISS) && 
            toting(ROD) && !cave_is_closing()) { 
        if (objs(FISSURE).prop) { 
            puts(GET_CSTR( CSTR_THE_CRYSTAL_BRIDGE_HAS_VANISHED1 )); 
            objs(FISSURE).prop = 0; 
        } else { 
            puts(GET_CSTR( CSTR_CRYSTAL_BRIDGE_NOW_SPANS_THE_FISSURE )); 
            objs(FISSURE).prop = 1; 
        } 
    } else if (toting(obj) || (obj == ROD && toting(ROD2))) { 
        puts(GET_CSTR( CSTR_NOTHING_HAPPENS )); 
    } else { 
        puts(GET_CSTR( CSTR_YOU_AREN_CARRYING_IT6 )); 
    } 
} 
 
void attempt_blast(Location loc)  /* section 99 in Knuth */ 
{ 
    if (closed && objs(ROD2).prop >= 0) { 
        if (here(ROD2, loc)) { 
            bonus = 25; 
            puts(GET_CSTR( CSTR_THERE_IS_LOUD_EXPLOSION_AND_YOU_ARE1 ) SOFT_NL 
                 GET_CSTR( CSTR_WALLS_OF_THE_ROOM )); 
        } else if (loc == R_NEEND) { 
            bonus = 30; 
            puts(GET_CSTR( CSTR_THERE_IS_LOUD_EXPLOSION_AND_TWENTY3 ) SOFT_NL 
                 GET_CSTR( CSTR_WALL_BURYING_THE_SNAKES_IN_THE_RUBBLE1 ) SOFT_NL 
                 GET_CSTR( CSTR_IN_THROUGH_THE_HOLE_DESTROYING1 )); 
        } else { 
            bonus = 45; 
            puts(GET_CSTR( CSTR_THERE_IS_LOUD_EXPLOSION_AND_TWENTY2 ) SOFT_NL 
                 GET_CSTR( CSTR_WALL_BURYING_THE_DWARVES_IN_THE_RUBBLE1 ) SOFT_NL 
                 GET_CSTR( CSTR_AND_FIND_YOURSELF_IN_THE_MAIN_OFFICE1 ) SOFT_NL 
                 GET_CSTR( CSTR_FRIENDLY_ELVES_CARRY_THE_CONQUERING1 )); 
        } 
        quit(); 
    } else { 
        puts(GET_CSTR( CSTR_BLASTING_REQUIRES_DYNAMITE1 )); 
    } 
} 
 
void attempt_rub(ObjectWord obj)  /* section 99 in Knuth */ 
{ 
    if (obj == LAMP) { 
        puts(GET_CSTR( CSTR_RUBBING_THE_ELECTRIC_LAMP_IS_NOT1 ) SOFT_NL 
             GET_CSTR( CSTR_NOTHING_EXCITING_HAPPENS1 )); 
    } else { 
        puts(GET_CSTR( CSTR_PECULIAR_NOTHING_UNEXPECTED_HAPPENS1 )); 
    } 
} 
 
void attempt_find(ObjectWord obj, Location loc)  /* section 100 in Knuth */ 
{ 
    if (toting(obj)) { 
        puts(GET_CSTR( CSTR_YOU_ARE_ALREADY_CARRYING_IT3 )); 
    } else if (closed) { 
        puts(GET_CSTR( CSTR_DARESAY_WHATEVER_YOU_WANT_IS_AROUND1 )); 
    } else { 
        bool its_right_here = false; 
        if (is_at_loc(obj, loc)) { 
            its_right_here = true; 
        } else if (obj != NOTHING && obj == bottle_contents() && there(BOTTLE, loc)) { 
            its_right_here = true; 
        } else if (obj == WATER && has_water(loc)) { 
            its_right_here = true; 
        } else if (obj == OIL && has_oil(loc)) { 
            its_right_here = true; 
        } else if (obj == DWARF && dwarf_at(loc)) { 
            its_right_here = true; 
        } 
        if (its_right_here) { 
            puts(GET_CSTR( CSTR_BELIEVE_WHAT_YOU_WANT_IS_RIGHT_HERE1 )); 
        } else { 
            puts(GET_CSTR( CSTR_CAN_ONLY_TELL_YOU_WHAT_YOU_SEE_AS_YOU2 ) SOFT_NL 
                 GET_CSTR( CSTR_THINGS_CANNOT_TELL_YOU_WHERE_REMOTE2 )); 
        } 
    } 
} 
 
void attempt_break(ObjectWord obj, Location loc)  /* section 101 in Knuth */ 
{ 
    if (obj == VASE && objs(VASE).prop == 0) { 
        if (toting(VASE)) 
            drop(VASE, loc); 
        puts(GET_CSTR( CSTR_YOU_HAVE_TAKEN_THE_VASE_AND_HURLED_IT1 )); 
        objs(VASE).prop = 1;  /* worthless shards */ 
        immobilize(VASE); 
    } else if (obj == MIRROR) { 
        if (closed) { 
            puts(GET_CSTR( CSTR_YOU_STRIKE_THE_MIRROR_RESOUNDING_BLOW1 ) SOFT_NL 
                 GET_CSTR( CSTR_MYRIAD_TINY_FRAGMENTS1 )); 
            dwarves_upset(); 
        } else { 
            puts(GET_CSTR( CSTR_IT_IS_TOO_FAR_UP_FOR_YOU_TO_REACH2 )); 
        } 
    } else { 
        puts(GET_CSTR( CSTR_IT_IS_BEYOND_YOUR_POWER_TO_DO_THAT1 )); 
    } 
} 
 
void attempt_wake(ObjectWord obj)  /* section 101 in Knuth */ 
{ 
    if (closed && obj == DWARF) { 
        puts(GET_CSTR( CSTR_YOU_PROD_THE_NEAREST_DWARF_WHO_WAKES_UP1 ) SOFT_NL 
             GET_CSTR( CSTR_YOU_CURSES_AND_GRABS_FOR_HIS_AXE1 )); 
        dwarves_upset(); 
    } 
    puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS )); 
} 
 
void attempt_off(Location loc)  /* section 102 in Knuth */ 
{ 
    if (!here(LAMP, loc)) { 
        puts(GET_CSTR( CSTR_YOU_HAVE_NO_SOURCE_OF_LIGHT2 )); 
    } else { 
        objs(LAMP).prop = 0; 
        puts(GET_CSTR( CSTR_YOUR_LAMP_IS_NOW_OFF1 )); 
        if (now_in_darkness(loc)) 
            puts(pitch_dark_msg); 
    } 
} 
 
void kill_a_dwarf(Location loc) 
{ 
    static bool first_time = true; 
    if (first_time) { 
        puts(GET_CSTR( CSTR_YOU_KILLED_LITTLE_DWARF_THE_BODY1 ) SOFT_NL 
             GET_CSTR( CSTR_BLACK_SMOKE )); 
        first_time = false; 
    } else { 
        puts(GET_CSTR( CSTR_YOU_KILLED_LITTLE_DWARF1 )); 
    } 
 
    int j; 
    for (j=1; j <= 5; ++j) { 
        if (dwarves[j].loc == loc) break; 
    } 
    assert(j <= 5); 
    dwarves[j].loc = R_LIMBO;  /* Once killed, a dwarf never comes back. */ 
    dwarves[j].seen = false; 
} 
 
void throw_axe_at_dwarf(Location loc)  /* section 163 in Knuth */ 
{ 
    if (ran(3) < 2) { 
        kill_a_dwarf(loc); 
    } else { 
        puts(GET_CSTR( CSTR_YOU_ATTACK_LITTLE_DWARF_BUT_HE_DODGES1 )); 
    } 
} 
 
/* Return true if we don't know what to fill. */ 
bool attempt_fill(ObjectWord obj, Location loc)  /* sections 110--111 in Knuth */ 
{ 
    if (obj == VASE) { 
        if (!has_oil(loc) && !has_water(loc)) { 
            puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_WITH_WHICH_TO2 )); 
        } else if (!toting(VASE)) { 
            puts(GET_CSTR( CSTR_YOU_AREN_CARRYING_IT7 )); 
        } else { 
            /* In Crowther and Woods' original, after shattering the vase this 
             * way, we GOTO the generic "drop" code. This produces a silly 
             * combination of messages --- and repairs the vase! --- if the 
             * pillow is on the ground next to you as you fill the vase. 
             * In Long's "Adventure 6", we skip the pillow-checking code, but 
             * still end up in the default handler, which would normally 
             * print "Dropped." but in this instance prints "There is nothing 
             * here with which to fill the vase." */ 
            puts(GET_CSTR( CSTR_THE_SUDDEN_CHANGE_IN_TEMPERATURE_HAS1 )); 
            objs(VASE).prop = 1;  /* worthless shards */ 
            drop(VASE, loc); 
            immobilize(VASE); 
        } 
    } else if (!here(BOTTLE, loc)) { 
        if (obj == NOTHING) 
            return true; 
        puts(GET_CSTR( CSTR_YOU_CAN_FILL_THAT )); 
    } else if (obj != NOTHING && obj != BOTTLE) { 
        puts(GET_CSTR( CSTR_YOU_CAN_FILL_THAT )); 
    } else if (bottle_contents() != NOTHING) { 
        puts(GET_CSTR( CSTR_YOUR_BOTTLE_IS_ALREADY_FULL1 )); 
    } else if (has_oil(loc)) { 
        puts(GET_CSTR( CSTR_YOUR_BOTTLE_IS_NOW_FULL_OF_OIL1 )); 
        objs(BOTTLE).prop = 2; 
        if (toting(BOTTLE)) objs(OIL).place = R_INHAND; 
    } else if (has_water(loc)) { 
        puts(GET_CSTR( CSTR_YOUR_BOTTLE_IS_NOW_FULL_OF_WATER1 )); 
        objs(BOTTLE).prop = 0; 
        if (toting(BOTTLE)) objs(WATER).place = R_INHAND; 
    } else { 
        puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_WITH_WHICH_TO3 )); 
    } 
    return false;  /* just continue normally */ 
} 
 
void attempt_feed(ObjectWord obj, Location loc)  /* section 129 in Knuth */ 
{ 
    switch (obj) { 
        case BIRD: 
            puts(GET_CSTR( CSTR_IT_NOT_HUNGRY_IT_MERELY_PININ_FOR_THE1 ) SOFT_NL 
                 GET_CSTR( CSTR_HAVE_NO_BIRD_SEED )); 
            break; 
        case TROLL: 
            puts(GET_CSTR( CSTR_GLUTTONY_IS_NOT_ONE_OF_THE_TROLL_VICES1 )); 
            break; 
        case DRAGON: 
            if (objs(DRAGON).prop) { 
                puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS ));  /* reject feeding the dead dragon */ 
            } else { 
                puts(GET_CSTR( CSTR_THERE_NOTHING_HERE_IT_WANTS_TO_EAT3 )); 
            } 
            break; 
        case SNAKE: 
            if (!closed && here(BIRD, loc)) { 
                destroy(BIRD); 
                objs(BIRD).prop = 0; 
                ++lost_treasures; 
                puts(GET_CSTR( CSTR_THE_SNAKE_HAS_NOW_DEVOURED_YOUR_BIRD1 )); 
            } else { 
                puts(GET_CSTR( CSTR_THERE_NOTHING_HERE_IT_WANTS_TO_EAT5 )); 
            } 
            break; 
        case BEAR: 
            if (here(FOOD, loc)) { 
                assert(objs(BEAR).prop == 0); 
                destroy(FOOD); 
                objs(BEAR).prop = 1; 
                objs(AXE).prop = 0; 
                mobilize(AXE);  /* if it was immobilized by the bear */ 
                puts(GET_CSTR( CSTR_THE_BEAR_EAGERLY_WOLFS_DOWN_YOUR_FOOD1 ) SOFT_NL 
                     GET_CSTR( CSTR_DOWN_CONSIDERABLY_AND_EVEN_BECOMES1 )); 
            } else if (objs(BEAR).prop == 0) { 
                puts(GET_CSTR( CSTR_THERE_NOTHING_HERE_IT_WANTS_TO_EAT4 )); 
            } else if (objs(BEAR).prop == 3) { 
                puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS )); 
            } else { 
                /* The bear is tame; therefore the food is gone. */ 
                puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_TO_EAT2 )); 
            } 
            break; 
        case DWARF: 
            if (here(FOOD, loc)) { 
                ++dflag; 
                puts(GET_CSTR( CSTR_YOU_FOOL_DWARVES_EAT_ONLY_COAL_NOW_YOU1 )); 
            } else { 
                puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_TO_EAT3 )); 
            } 
            break; 
        default: 
            puts(GET_CSTR( CSTR_GAME_WOULD_YOU_CARE_TO_EXPLAIN_HOW2 )); 
            break; 
    } 
} 
 
void attempt_open_or_close(ActionWord verb, ObjectWord obj, Location loc)  /* sections 130--134 in Knuth */ 
{ 
    bool verb_is_open = (verb == OPEN);  /* otherwise it's CLOSE */ 
    switch (obj) { 
        case OYSTER: 
        case CLAM: { 
            CTEXT clam_oyster = (obj == CLAM ? GET_CSTR( CSTR_CLAM ) : GET_CSTR( CSTR_OYSTER )); 
            if (!verb_is_open) { 
                puts(GET_CSTR( CSTR_WHAT )); 
            } else if (!toting(TRIDENT)) { 
                puts(GET_CSTR( CSTR_YOU_DON_HAVE_ANYTHING_WITH_WHICH_TO1 )); 
                puts( clam_oyster ); 
                puts(".\n"); 
            } else if (toting(obj)) { 
                puts_nocr(GET_CSTR( CSTR_ADVISE_YOU_TO_PUT_DOWN_THE1 )); 
                puts_nocr( clam_oyster );                 
                puts_nocr(GET_CSTR( CSTR_BEFORE_OPENING_IT )); 
                puts( (obj == CLAM ? GET_CSTR( CSTR_STRAIN ) : GET_CSTR( CSTR_WRENCH ))); 
                 
            } else if (obj == CLAM) { 
                /* The pearl will appear in the cul-de-sac no matter where 
                 * we are; we don't allow the player to carry the clam out 
                 * of the Shell Room area. */ 
                destroy(CLAM); 
                drop(OYSTER, loc); 
                drop(PEARL, R_SAC); 
                puts(GET_CSTR( CSTR_GLISTENING_PEARL_FALLS_OUT_OF_THE_CLAM1 ) SOFT_NL 
                     GET_CSTR( CSTR_THIS_MUST_REALLY_BE_AN_OYSTER_NEVER_WAS1 ) SOFT_NL 
                     GET_CSTR( CSTR_BIVALVES_WHATEVER_IT_IS_IT_HAS_NOW1 )); 
            } else { 
                puts(GET_CSTR( CSTR_THE_OYSTER_CREAKS_OPEN_REVEALING1 ) SOFT_NL 
                     GET_CSTR( CSTR_IT_PROMPTLY_SNAPS_SHUT_AGAIN1 )); 
            } 
            break; 
        } 
        case GRATE: 
            if (!here(KEYS, loc)) { 
                puts(GET_CSTR( CSTR_YOU_HAVE_NO_KEYS )); 
            } else if (cave_is_closing()) { 
                /* Trying to get out through the grate after closing. */ 
                panic_at_closing_time(); 
            } else { 
                bool was_open = objs(GRATE).prop; 
                objs(GRATE).prop = verb_is_open; 
                switch (was_open + 2*verb_is_open) { 
                    case 0: puts(GET_CSTR( CSTR_IT_WAS_ALREADY_LOCKED3 )); break; 
                    case 1: puts(GET_CSTR( CSTR_THE_GRATE_IS_NOW_LOCKED1 )); break; 
                    case 2: puts(GET_CSTR( CSTR_THE_GRATE_IS_NOW_UNLOCKED1 )); break; 
                    case 3: puts(GET_CSTR( CSTR_IT_WAS_ALREADY_UNLOCKED2 )); break; 
                } 
            } 
            break; 
        case CHAIN: 
            if (!here(KEYS, loc)) { 
                puts(GET_CSTR( CSTR_YOU_HAVE_NO_KEYS )); 
            } else if (verb_is_open) { 
                /* UNLOCK CHAIN */ 
                if (objs(CHAIN).prop == 0) { 
                    puts(GET_CSTR( CSTR_IT_WAS_ALREADY_UNLOCKED3 )); 
                } else if (objs(BEAR).prop == 0) { 
                    puts(GET_CSTR( CSTR_THERE_IS_NO_WAY_TO_GET_PAST_THE_BEAR_TO1 ) SOFT_NL 
                         GET_CSTR( CSTR_PROBABLY_JUST_AS_WELL1 )); 
                } else { 
                    objs(CHAIN).prop = 0; 
                    mobilize(CHAIN); 
                    if (objs(BEAR).prop == 1) { 
                        objs(BEAR).prop = 2; 
                        mobilize(BEAR); 
                    } 
                    puts(GET_CSTR( CSTR_THE_CHAIN_IS_NOW_UNLOCKED1 )); 
                } 
            } else { 
                /* LOCK CHAIN */ 
                if (loc != R_BARR) { 
                    puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_TO_WHICH_THE1 )); 
                } else if (objs(CHAIN).prop) { 
                    puts(GET_CSTR( CSTR_IT_WAS_ALREADY_LOCKED2 )); 
                } else { 
                    objs(CHAIN).prop = 2; 
                    immobilize(CHAIN); 
                    if (toting(CHAIN)) drop(CHAIN, loc); 
                    puts(GET_CSTR( CSTR_THE_CHAIN_IS_NOW_LOCKED1 )); 
                } 
            } 
            break; 
        case KEYS: 
            puts(GET_CSTR( CSTR_YOU_CAN_LOCK_OR_UNLOCK_THE_KEYS1 )); 
            break; 
        case CAGE: 
            puts(GET_CSTR( CSTR_IT_HAS_NO_LOCK )); 
            break; 
        case RUSTY_DOOR: 
            if (objs(RUSTY_DOOR).prop) { 
                puts(ok); 
            } else { 
                /* Notice that CLOSE DOOR also gives this response. */ 
                puts(GET_CSTR( CSTR_THE_DOOR_IS_EXTREMELY_RUSTY_AND2 )); 
            } 
            break; 
        default: 
            puts(GET_CSTR( CSTR_DON_KNOW_HOW_TO_LOCK_OR_UNLOCK_SUCH1 )); 
            break; 
    } 
} 
 
/* This is some pretty baroque logic for an obscure case. 
 * We're deciding what the noun should be if the player types 
 * "READ" as a one-word command. If we return NOTHING, the 
 * main loop will branch to get_object; otherwise it will 
 * be handled as if we'd typed "READ <obj>". */ 
ObjectWord read_what(Location loc) 
{ 
    if (now_in_darkness(loc)) 
        return NOTHING;  /* can't read in the dark */ 
    if (closed && toting(OYSTER)) 
        return OYSTER; 
 
    bool magazines_here = here(MAG, loc); 
    if (here(TABLET, loc)) 
        return magazines_here ? NOTHING : TABLET; 
    if (here(MESSAGE, loc)) 
        return magazines_here ? NOTHING : MESSAGE; 
    return magazines_here ? MAG : NOTHING; 
} 
 
void attempt_read(ObjectWord obj)  /* section 135 in Knuth */ 
{ 
    switch (obj) { 
        case MAG: 
            puts(GET_CSTR( CSTR_AFRAID_THE_MAGAZINE_IS_WRITTEN_IN1 )); 
            break; 
        case TABLET: 
            puts(GET_CSTR( CSTR_CONGRATULATIONS_ON_BRINGING_LIGHT3 )); 
            break; 
        case MESSAGE: 
            puts(GET_CSTR( CSTR_THIS_IS_NOT_THE_MAZE_WHERE_THE_PIRATE3 )); 
            break; 
        case OYSTER: 
            if (closed && toting(OYSTER)) { 
                if (hints[1].given) { 
                    puts(GET_CSTR( CSTR_IT_SAYS_THE_SAME_THING_IT_DID_BEFORE1 )); 
                } else { 
                    offer(1); 
                } 
                break; 
            } 
            /* FALLTHROUGH */ 
        default: 
            puts(GET_CSTR( CSTR_AFRAID_DON_UNDERSTAND1 )); 
            break; 
    } 
} 
 
int check_noun_validity(ObjectWord obj, Location loc)  /* sections 90--91 in Knuth */ 
{ 
    if (toting(obj) || is_at_loc(obj, loc)) 
        return 0;  /* no problem */ 
    switch (obj) { 
        case GRATE: 
            if (loc < MIN_LOWER_LOC) { 
                switch (loc) { 
                    case R_ROAD: case R_VALLEY: case R_SLIT: 
                        return 'd';  /* try moving to DEPRESSION */ 
                    case R_COBBLES: case R_DEBRIS: case R_AWK: case R_BIRD: case R_SPIT: 
                        return 'e';  /* try moving to ENTRANCE */ 
                    default: 
                        break; 
                } 
            } 
            return 'c';  /* can't see it */ 
        case DWARF: 
            if (dflag >= 2 && dwarf_at(loc)) return 0; 
            return 'c';  /* can't see it */ 
        case PLANT: 
            if (is_at_loc(PLANT2, loc) && objs(PLANT2).prop != 0) { 
                return 'p';  /* obj = PLANT2 */ 
            } 
            return 'c';  /* can't see it */ 
        case KNIFE: { 
            /* You're allowed to try to GET KNIFE, but only once. The game 
             * informs you that the knife has vanished; and from then on, 
             * it never reappears. */ 
            static bool have_tried_to_get_knife = false; 
            if (have_tried_to_get_knife || (loc != last_knife_loc)) { 
                return 'c';  /* can't see it */ 
            } 
            puts(GET_CSTR( CSTR_THE_DWARVES_KNIVES_VANISH_AS_THEY1 )); 
            have_tried_to_get_knife = true; 
            return 'f';  /* action is finished */ 
        } 
        case ROD: 
            if (!here(ROD2, loc)) return 'c'; 
            return 'r';  /* obj = ROD2 */ 
        case WATER: 
            if (has_water(loc)) return 0; 
            if (here(BOTTLE, loc) && bottle_contents() == WATER) return 0; 
            return 'c'; 
        case OIL: 
            if (has_oil(loc)) return 0; 
            if (here(BOTTLE, loc) && bottle_contents() == OIL) return 0; 
            return 'c'; 
        default: 
            break;              // Suppress warning 
    } 
    return 'c'; 
} 
 
Instruction *determine_motion_instruction(Location loc, MotionWord mot) 
{ 
    Instruction *q; 
    for (q = start[loc]; q < start[loc+1]; ++q) { 
        if (is_forced(loc) || q->mot == mot) break; 
    } 
    if (q == start[loc+1]) { 
        return NULL;  /* newloc = loc at this point */ 
    } 
    while (true) { 
        const int cond = q->cond; 
        if (cond == 0) { 
            break;  /* the usual case */ 
        } else if (cond <= 100) { 
            if (pct(cond)) break;  /* dwarves won't take these routes */ 
        } else if (cond <= 200) { 
            if (toting(MIN_OBJ + cond%100)) break; 
        } else if (cond <= 300) { 
            if (is_at_loc(MIN_OBJ + cond%100, loc)) break; 
        } else { 
            if (objs(MIN_OBJ + cond%100).prop != (cond/100)-3) break; 
        } 
        /* The verb in this instruction matches the "mot" typed by the user, 
         * but the condition failed, either randomly (j < 100) or because 
         * the prerequisite wasn't satisfied. So instead of taking this 
         * instruction, we'll take the next one that is not a "ditto" of 
         * this one --- regardless of its "mot" field! */ 
        const Location dest = q->dest; 
        while (q->dest == dest && q->cond == cond) 
            ++q; 
    } 
    return q; 
} 
 
void report_inapplicable_motion(MotionWord mot, ActionWord verb) 
{ 
    if (mot == CRAWL) { 
        puts(GET_CSTR( CSTR_WHICH_WAY )); 
    } else if (mot == XYZZY || mot == PLUGH) { 
        puts(GET_CSTR( CSTR_NOTHING_HAPPENS )); 
    } else if (verb == FIND || verb == INVENTORY) { 
        /* This catches inputs such as "FIND BEDQUILT" or "INVENTORY WEST". */ 
        puts(GET_CSTR( CSTR_CAN_ONLY_TELL_YOU_WHAT_YOU_SEE_AS_YOU3 ) SOFT_NL 
             GET_CSTR( CSTR_THINGS_CANNOT_TELL_YOU_WHERE_REMOTE3 )); 
    } else { 
        switch (mot) { 
            case N: case S: case E: case W: case NE: case SE: case NW: case SW: 
            case U: case D: 
                puts(GET_CSTR( CSTR_THERE_IS_NO_WAY_TO_GO_IN_THAT_DIRECTION1 )); 
                break; 
            case IN: case OUT: 
                puts(GET_CSTR( CSTR_DON_KNOW_IN_FROM_OUT_HERE_USE_COMPASS1 ) SOFT_NL 
                     GET_CSTR( CSTR_IN_THE_GENERAL_DIRECTION_YOU_WANT_TO_GO1 )); 
                break; 
            case FORWARD: case LEFT: case RIGHT: 
                puts(GET_CSTR( CSTR_AM_UNSURE_HOW_YOU_ARE_FACING_USE1 )); 
                break; 
            default: 
                puts(GET_CSTR( CSTR_DON_KNOW_HOW_TO_APPLY_THAT_WORD_HERE1 )); 
                break; 
        } 
    } 
} 
 
void print_remark(int which) 
{ 
    switch (which) { 
        case 0: 
            puts(GET_CSTR( CSTR_YOU_DON_FIT_THROUGH_TWO_INCH_SLIT1 )); 
            break; 
        case 1: 
            puts(GET_CSTR( CSTR_YOU_CAN_GO_THROUGH_LOCKED_STEEL_GRATE1 )); 
            break; 
        case 2: 
            puts(GET_CSTR( CSTR_RESPECTFULLY_SUGGEST_YOU_GO_ACROSS1 )); 
            break; 
        case 3: 
            puts(GET_CSTR( CSTR_THERE_IS_NO_WAY_ACROSS_THE_FISSURE1 )); 
            break; 
        case 4: 
            puts(GET_CSTR( CSTR_YOU_CAN_FIT_THIS_FIVE_FOOT_CLAM_THROUGH1 )); 
            break; 
        case 5: 
            puts(GET_CSTR( CSTR_YOU_CAN_FIT_THIS_FIVE_FOOT_OYSTER1 )); 
            break; 
        case 6: 
            puts(GET_CSTR( CSTR_YOU_HAVE_CRAWLED_AROUND_IN_SOME_LITTLE2 ) SOFT_NL 
                 GET_CSTR( CSTR_MAIN_PASSAGE )); 
            break; 
        case 7: 
            puts(GET_CSTR( CSTR_YOU_HAVE_CRAWLED_AROUND_IN_SOME_LITTLE3 ) SOFT_NL 
                 GET_CSTR( CSTR_BLOCKED_BY_RECENT_CAVE_IN_YOU_ARE_NOW1 )); 
            break; 
        case 8: 
            puts(GET_CSTR( CSTR_IT_IS_TOO_FAR_UP_FOR_YOU_TO_REACH3 )); 
            break; 
        case 9: 
            puts(GET_CSTR( CSTR_THE_DOOR_IS_EXTREMELY_RUSTY_AND3 )); 
            break; 
        case 10: 
            puts(GET_CSTR( CSTR_THE_DRAGON_LOOKS_RATHER_NASTY_YOU_BEST1 )); 
            break; 
        case 11: 
            puts(GET_CSTR( CSTR_THE_TROLL_REFUSES_TO_LET_YOU_CROSS1 )); 
            break; 
        case 12: 
            puts(GET_CSTR( CSTR_THERE_IS_NO_LONGER_ANY_WAY_ACROSS_THE1 )); 
            break; 
        case 13: 
            puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS )); 
            break; 
        case 14: 
            puts(GET_CSTR( CSTR_THE_CRACK_IS_FAR_TOO_SMALL_FOR_YOU_TO1 )); 
            break; 
        case 15: 
            puts(GET_CSTR( CSTR_THE_DOME_IS_UNCLIMBABLE1 )); 
            break; 
        case 16: 
            puts(GET_CSTR( CSTR_YOU_CAN_GET_BY_THE_SNAKE1 )); 
            break; 
        case 17: 
            puts(GET_CSTR( CSTR_THE_STREAM_FLOWS_OUT_THROUGH_PAIR_OF1 ) SOFT_NL 
                 GET_CSTR( CSTR_IT_WOULD_BE_ADVISABLE_TO_USE_THE_EXIT1 )); 
            break; 
    } 
} 
 
MotionWord try_going_back_to(Location l, Location from) 
{ 
    /* Interestingly, the BACK command does not simply move the player 
     * back to oldloc. Instead, it attempts to trace a path connecting 
     * loc to oldloc; if no such passage exists, we fail to move. If 
     * such a passage does exist, then we react as if the player had 
     * typed the appropriate motion-word in the first place. 
     * Notice that Woods' code makes an effort to deal with FORCED_MOVE 
     * locations, but doesn't succeed 100 percent. ("l" is set to oldloc, 
     * or to oldoldloc if oldloc is a FORCED_MOVE location.) 
     *     R_SWSIDE : CROSS -> R_TROLL -> R_NESIDE : BACK 
     * gives "You can't get there from here." as apparently intended, 
     * because R_TROLL is a special location. 
     *     R_WPIT : CLIMB -> R_CHECK -> R_UPNOUT -> R_WPIT : BACK 
     * unintentionally gives "There is nothing here to climb.", because 
     * oldoldloc is R_CHECK, and there *is* a passage connecting our 
     * current location (R_WPIT) to R_CHECK, so we take it again. 
     *     R_WPIT : CLIMB -> R_CHECK -> R_DIDIT -> R_W2PIT : BACK 
     * unintentionally gives "You can't get there from here.", because 
     * oldoldloc is R_CHECK, and there is no passage connecting our 
     * current location (R_W2PIT) to R_CHECK. */ 
 
    if (l == from) { 
        puts(GET_CSTR( CSTR_SORRY_BUT_NO_LONGER_SEEM_TO_REMEMBER1 )); 
        return NOWHERE; 
    } 
    for (Instruction *q = start[from]; q < start[from+1]; ++q) { 
        /* Take the first exit that goes to "l", either directly... */ 
        Location ll = q->dest; 
        if (ll == l) 
            return q->mot; 
        /* ...or via a single forced move. */ 
        if (is_forced(ll) && start[ll]->dest == l) 
            return q->mot; 
    } 
    puts(GET_CSTR( CSTR_YOU_CAN_GET_THERE_FROM_HERE1 )); 
    return NOWHERE; 
} 
 
void collapse_the_troll_bridge(void) 
{ 
    puts(GET_CSTR( CSTR_JUST_AS_YOU_REACH_THE_OTHER_SIDE_THE1 ) SOFT_NL 
         GET_CSTR( CSTR_WEIGHT_OF_THE_BEAR_WHO_WAS_STILL1 ) SOFT_NL 
         GET_CSTR( CSTR_SCRABBLE_DESPERATELY_FOR_SUPPORT_BUT1 ) SOFT_NL 
         GET_CSTR( CSTR_STUMBLE_BACK_AND_FALL_INTO_THE_CHASM1 )); 
    objs(CHASM).prop = 1; 
    objs(TROLL).prop = 2; 
    objs(BEAR).prop = 3;  /* the bear is dead */ 
    drop(BEAR, R_SWSIDE); 
    immobilize(BEAR); 
    /* If you collapse the troll bridge without seeing the spices, we'll 
     * increment lost_treasures so you can still get to the endgame. 
     * However, if you threw the trident to the troll before opening the 
     * oyster, you're strictly out of luck. And in fact if you pick up 
     * a treasure in the dark, you'll never "see" it until you drop it 
     * in a lighted area; so there can be arbitrarily many unseen 
     * treasures on the far side of the bridge, if the player is doing 
     * it deliberately. But we don't care about that case. 
     */ 
    if (objs(SPICES).prop < 0 && objs(SPICES).place >= R_NESIDE) 
        ++lost_treasures; 
} 
 
/* Modify newloc in place, and return true if the player died crossing the troll bridge. */ 
bool determine_next_newloc(Location loc, Location *newloc, MotionWord mot, ActionWord verb) 
{ 
    Instruction *q = determine_motion_instruction(loc, mot); 
    if (q == NULL) { 
        report_inapplicable_motion(mot, verb); 
        return false; 
    } 
    *newloc = q->dest; 
 
    if (*newloc >= FIRST_REMARK) { 
        print_remark(*newloc - FIRST_REMARK); 
        *newloc = loc; 
    } else if (*newloc == R_PPASS) { 
        *newloc = attempt_plover_passage(loc); 
    } else if (*newloc == R_PDROP) { 
        drop(EMERALD, loc); 
        *newloc = R_Y2 + R_PLOVER - loc; 
    } else if (*newloc == R_TROLL) { 
        /* Troll bridge crossing is treated as a special motion so 
         * that dwarves won't wander across and encounter the bear. 
         * You can get here only if TROLL is in limbo but NO_TROLL has 
         * taken its place. Moreover, if you're on the southwest side, 
         * objs(TROLL).prop will be nonzero. If objs(TROLL).prop is 1, 
         * you've crossed since paying, or you've stolen away the payment. 
         */ 
        if (objs(TROLL).prop == 1) { 
            /* Block the troll bridge and stay put. */ 
            objs(TROLL).prop = 0; 
            destroy(NO_TROLL); destroy(NO_TROLL_); 
            drop(TROLL, R_SWSIDE); drop(TROLL_, R_NESIDE); 
            juggle(CHASM); juggle(CHASM_); 
            puts(GET_CSTR( CSTR_THE_TROLL_STEPS_OUT_FROM_BENEATH_THE1 )); 
            *newloc = loc;  /* stay put */ 
        } else { 
            *newloc = R_NESIDE + R_SWSIDE - loc;  /* cross it */ 
            if (objs(TROLL).prop == 0) 
                objs(TROLL).prop = 1; 
            if (toting(BEAR)) { 
                assert(*newloc == R_SWSIDE); 
                collapse_the_troll_bridge(); 
                return true;  /* goto death */ 
            } 
        } 
    } 
 
    return false; 
} 
 
void print_message(MessageWord msg) 
{ 
    switch (msg) { 
        case ABRA: 
            puts(GET_CSTR( CSTR_GOOD_TRY_BUT_THAT_IS_AN_OLD_WORN_OUT1 )); 
            break; 
        case HELP: 
            puts(GET_CSTR( CSTR_KNOW_OF_PLACES_ACTIONS_AND_THINGS_MOST1 ) SOFT_NL 
                 GET_CSTR( CSTR_DESCRIBES_PLACES_AND_IS_USED_TO_MOVE1 ) SOFT_NL 
                 GET_CSTR( CSTR_LIKE_FOREST_BUILDING_DOWNSTREAM1 ) SOFT_NL 
                 GET_CSTR( CSTR_UP_OR_DOWN_KNOW_ABOUT_FEW_SPECIAL1 ) SOFT_NL 
                 GET_CSTR( CSTR_HIDDEN_IN_THE_CAVE_THESE_OBJECTS_CAN_BE1 ) SOFT_NL 
                 GET_CSTR( CSTR_THE_ACTION_WORDS_THAT_KNOW_USUALLY_YOU1 ) SOFT_NL 
                 GET_CSTR( CSTR_OBJECT_AND_ACTION_WORDS_IN_EITHER1 ) SOFT_NL 
                 GET_CSTR( CSTR_THE_OBJECT_FROM_THE_VERB_ALONE_SOME1 ) SOFT_NL 
                 GET_CSTR( CSTR_PARTICULAR_INVENTORY_IMPLIES_TAKE1 ) SOFT_NL 
                 GET_CSTR( CSTR_GIVE_YOU_LIST_OF_WHAT_YOU_RE_CARRYING1 ) SOFT_NL 
                 GET_CSTR( CSTR_EFFECTS_FOR_INSTANCE_THE_ROD_SCARES1 ) SOFT_NL 
                 GET_CSTR( CSTR_TROUBLE_MOVING_JUST_NEED_TO_TRY_FEW1 ) SOFT_NL 
                 GET_CSTR( CSTR_TRYING_UNSUCCESSFULLY_TO_MANIPULATE1 ) SOFT_NL 
                 GET_CSTR( CSTR_BEYOND_THEIR_OR_MY_CAPABILITIES_AND1 ) SOFT_NL 
                 GET_CSTR( CSTR_DIFFERENT_TACK_TO_SPEED_THE_GAME_YOU1 ) SOFT_NL 
                 GET_CSTR( CSTR_DISTANCES_WITH_SINGLE_WORD_FOR1 ) SOFT_NL 
                 GET_CSTR( CSTR_YOU_TO_THE_BUILDING_FROM_ANYWHERE1 ) SOFT_NL 
                 GET_CSTR( CSTR_FOREST_ALSO_NOTE_THAT_CAVE_PASSAGES1 ) SOFT_NL 
                 GET_CSTR( CSTR_ROOM_TO_THE_NORTH_DOES_NOT_GUARANTEE1 ) SOFT_NL 
                 GET_CSTR( CSTR_GOOD_LUCK )); 
            break; 
        case TREES: 
            puts(GET_CSTR( CSTR_THE_TREES_OF_THE_FOREST_ARE_LARGE1 ) SOFT_NL 
                 GET_CSTR( CSTR_OCCASIONAL_GROVE_OF_PINE_OR_SPRUCE1 ) SOFT_HYPHEN 
                 GET_CSTR( CSTR_GROWTH_LARGELY_BIRCH_AND_ASH_SAPLINGS1 ) SOFT_NL 
                 GET_CSTR( CSTR_VARIOUS_SORTS_THIS_TIME_OF_YEAR1 ) SOFT_NL 
                 GET_CSTR( CSTR_ALL_THE_LEAVES_BUT_TRAVEL_IS_QUITE_EASY1 ) SOFT_NL 
                 GET_CSTR( CSTR_SPRUCE_AND_BERRY_BUSHES1 )); 
            break; 
        case DIG: 
            puts(GET_CSTR( CSTR_DIGGING_WITHOUT_SHOVEL_IS_QUITE1 ) SOFT_NL 
                 GET_CSTR( CSTR_PROGRESS_IS_UNLIKELY1 )); 
            break; 
        case LOST: 
            puts(GET_CSTR( CSTR_AS_CONFUSED_AS_YOU_ARE1 )); 
            break; 
        case MIST: 
            puts(GET_CSTR( CSTR_MIST_IS_WHITE_VAPOR_USUALLY_WATER_SEEN1 ) SOFT_NL 
                 GET_CSTR( CSTR_CAVERNS_IT_CAN_BE_FOUND_ANYWHERE_BUT_IS1 ) SOFT_NL 
                 GET_CSTR( CSTR_PIT_LEADING_DOWN_TO_WATER1 )); 
            break; 
        case FUCK: 
            puts(GET_CSTR( CSTR_WATCH_IT )); 
            break; 
        case STOP: 
            puts(GET_CSTR( CSTR_DON_KNOW_THE_WORD_STOP_USE_QUIT_IF_YOU1 )); 
            break; 
        case INFO: 
            puts(GET_CSTR( CSTR_IF_YOU_WANT_TO_END_YOUR_ADVENTURE_EARLY1 ) SOFT_NL 
                 GET_CSTR( CSTR_CREDIT_FOR_TREASURE_YOU_MUST_HAVE_LEFT1 ) SOFT_NL 
                 GET_CSTR( CSTR_THOUGH_YOU_GET_PARTIAL_CREDIT_JUST_FOR1 ) SOFT_NL 
                 GET_CSTR( CSTR_FOR_GETTING_KILLED_OR_FOR_QUITTING1 ) SOFT_NL 
                 GET_CSTR( CSTR_THERE_ARE_ALSO_POINTS_BASED_ON_HOW_MUCH1 ) SOFT_NL 
                 GET_CSTR( CSTR_MANAGED_TO_EXPLORE_IN_PARTICULAR1 ) SOFT_NL 
                 GET_CSTR( CSTR_GETTING_IN_TO_DISTINGUISH_THE1 ) SOFT_NL 
                 GET_CSTR( CSTR_AND_THERE_ARE_OTHER_WAYS_TO_DETERMINE1 ) SOFT_NL 
                 GET_CSTR( CSTR_OF_THE_MORE_HARROWING_SECTIONS_IF_YOU1 ) SOFT_NL 
                 GET_CSTR( CSTR_TREASURES_JUST_KEEP_EXPLORING_FOR1 ) SOFT_NL 
                 GET_CSTR( CSTR_HAPPENS_YOU_HAVEN_FOUND_THEM_ALL_YET_IF1 ) SOFT_NL 
                 GET_CSTR( CSTR_DOES_HAPPEN_IT_MEANS_YOU_RE_GETTING1 ) SOFT_NL 
                 GET_CSTR( CSTR_TO_GARNER_MANY_MORE_POINTS_IN_THE1 ) SOFT_NL 
                 GET_CSTR( CSTR_MAY_OCCASIONALLY_OFFER_HINTS_IF_YOU1 ) SOFT_NL 
                 GET_CSTR( CSTR_IF_DO_LL_WARN_YOU_IN_ADVANCE_HOW_MUCH_IT1 ) SOFT_NL 
                 GET_CSTR( CSTR_TO_ACCEPT_THE_HINTS_FINALLY_TO_SAVE1 ) SOFT_NL 
                 GET_CSTR( CSTR_WHICH_TELLS_ME_NEVER_TO_REPEAT_THE_FULL1 ) SOFT_NL 
                 GET_CSTR( CSTR_UNLESS_YOU_EXPLICITLY_ASK_ME_TO1 )); 
            break; 
        case SWIM: 
            puts(GET_CSTR( CSTR_DON_KNOW_HOW )); 
            break; 
    } 
} 
 
void simulate_an_adventure(void) 
{ 
    Location oldoldloc, oldloc, loc, newloc; 
    MotionWord mot = NOWHERE;  /* currently specified motion */ 
    ActionWord verb = NOTHING;  /* currently specified action */ 
    ActionWord oldverb;  /* verb before it was changed */ 
    ObjectWord obj = NOTHING;  /* currently specified object, if any */ 
    ObjectWord oldobj;  /* former value of obj */ 
    bool was_dark = false; 
    int look_count = 0; 
 
    oldoldloc = oldloc = loc = newloc = R_ROAD; 
 
    while (true) { 
        /* Check for interference with the proposed move to newloc. */ 
        if (cave_is_closing() && newloc < MIN_IN_CAVE && newloc != R_LIMBO) { 
            panic_at_closing_time(); 
            newloc = loc; 
        } else if (newloc != loc && !forbidden_to_pirate(loc)) { 
            /* Stay in loc if a dwarf is blocking the way to newloc */ 
            for (int j=1; j <= 5; ++j) { 
                if (dwarves[j].seen && dwarves[j].oldloc == newloc) { 
                    puts(GET_CSTR( CSTR_LITTLE_DWARF_WITH_BIG_KNIFE_BLOCKS1 )); 
                    newloc = loc; break; 
                } 
            } 
        } 
        loc = newloc;  /* hey, we actually moved you */ 
        if (move_dwarves_and_pirate(loc)) { 
            oldoldloc = loc; 
            goto death; 
        } 
    commence: 
        if (loc == R_LIMBO) goto death; 
        switch (look_around(loc, now_in_darkness(loc), was_dark)) { 
            case 'p': goto pitch_dark; 
            case 't': goto try_move; 
            default: break; 
        } 
        while (true) { 
            int k; 
            verb = oldverb = NOTHING; 
            oldobj = obj; 
            obj = NOTHING; 
        cycle: 
            maybe_give_a_hint(loc, oldloc, oldoldloc, oldobj); 
            was_dark = now_in_darkness(loc); 
            adjustments_before_listening(loc); 
            listen(); 
        pre_parse: 
            ++turns; 
            if (verb == SAY) { 
                /* Maybe you said "say" and we said "say what?" and you replied 
                 * with two things to say. Then we assume you don't really want 
                 * us to say anything. Section 82 in Knuth. */ 
                if (*word2 != '\0') { 
                    verb = NOTHING; 
                } else { 
                    goto transitive; 
                } 
            } 
 
            /* Just after every command you give, we make the foobar counter 
             * negative if you're on track, otherwise we zero it. 
             * This is section 138 in Knuth. */ 
            foobar = (foobar > 0) ? -foobar : 0; 
 
            if (check_clocks_and_lamp(loc)) { 
                /* The cave just closed! */ 
                loc = oldloc = R_NEEND; 
                mot = NOWHERE; 
                goto try_move; 
            } 
 
            /* Handle additional special cases of input (Knuth sections 83, 105) */ 
            if (streq(word1, GET_CSTR( CSTR_ENTER ))) { 
                if (streq(word2, GET_CSTR( CSTR_WATER )) || streq(word2, GET_CSTR( CSTR_STREA ))) { 
                    if (has_water(loc)) { 
                        puts(GET_CSTR( CSTR_YOUR_FEET_ARE_NOW_WET1 )); 
                    } else { 
                        puts(GET_CSTR( CSTR_WHERE )); 
                    } 
                    continue; 
                } else if (*word2 != '\0') { 
                    /* ENTER TUNNEL becomes just TUNNEL. */ 
                    shift_words(); 
                    goto parse; 
                } 
            } 
            if (streq(word1, GET_CSTR( CSTR_WATER )) || streq(word1, GET_CSTR( CSTR_OIL ))) { 
                /* Sometimes "water" and "oil" are used as verbs. */ 
                if (streq(word2, GET_CSTR( CSTR_PLANT )) && there(PLANT, loc)) 
                    strcpy(word2, GET_CSTR( CSTR_POUR )); 
                if (streq(word2, GET_CSTR( CSTR_DOOR )) && there(RUSTY_DOOR, loc)) 
                    strcpy(word2, GET_CSTR( CSTR_POUR )); 
            } 
 
        parse: 
            advise_about_going_west(word1); 
            k = lookup(word1); 
            switch (word_class(k)) { 
                case WordClass_None: 
                    puts(GET_CSTR( CSTR_SORRY_DON_KNOW_THAT_WORD1 )); 
                    // TODO: word1); 
                    goto cycle; 
                case WordClass_Motion: 
                    mot = k; 
                    goto try_move; 
                case WordClass_Object: 
                    obj = k; 
                    /* Make sure obj is meaningful at the current location. 
                     * When you specify an object, it must be here, unless 
                     * the verb is already known to be FIND or INVENTORY. 
                     * A few other special cases are permitted; for example, 
                     * water and oil might be present inside the bottle or 
                     * as a feature of the location. */ 
                    switch (check_noun_validity(obj, loc)) { 
                        case 'c': goto cant_see_it; 
                        case 'd': mot = DEPRESSION; goto try_move; 
                        case 'e': mot = ENTRANCE; goto try_move; 
                        case 'f': continue; 
                        case 'p': obj = PLANT2; break; 
                        case 'r': obj = ROD2; break; 
                        /* case 0: break; */ 
                    } 
                    if (*word2 != '\0') break; 
                    if (verb != NOTHING) goto transitive; 
                    puts(GET_CSTR( CSTR_WHAT_DO_YOU_WANT_TO_DO_WITH_IT1 )); 
                    // TODO: the %s?\n", word1); 
                    goto cycle; 
                case WordClass_Action: 
                    verb = k; 
                    if (verb == SAY) { 
                        /* "FOO SAY" results in the incorrect message 
                         * "Say what?" instead of "Okay, "foo"". */ 
                        if (*word2 == '\0') goto intransitive; 
                        goto transitive; 
                    } 
                    if (*word2 != '\0') break; 
                    if (obj != NOTHING) goto transitive; 
                    else goto intransitive; 
                case WordClass_Message: 
                    print_message(k); 
                    continue; 
            } 
 
            shift_words(); 
            goto parse; 
 
        intransitive: 
            switch (verb) { 
                case GO: 
                    puts(GET_CSTR( CSTR_WHERE )); 
                    continue; 
                case RELAX: 
                    puts(ok); 
                    continue; 
                case ON: case OFF: case POUR: case FILL: case DRINK: case BLAST: case KILL: 
                    goto transitive; 
                case TAKE: { 
                    /* TAKE makes sense by itself if there's only one possible thing to take. */ 
                    ObjectWord object_here = places[loc].objects; 
                    if (dwarf_at(loc)) 
                        goto get_object; 
                    if (object_here != NOTHING && objs(object_here).link == NOTHING) { 
                        obj = object_here; 
                        goto transitive; 
                    } 
                    goto get_object; 
                } 
                case EAT: 
                    if (!here(FOOD, loc)) 
                        goto get_object; 
                    obj = FOOD; 
                    goto transitive; 
                case OPEN: case CLOSE: 
                    if (is_at_loc(GRATE, loc)) { 
                        obj = GRATE; 
                    } else if (there(RUSTY_DOOR, loc)) { 
                        obj = RUSTY_DOOR; 
                    } else if (here(CLAM, loc)) { 
                        obj = CLAM; 
                    } else if (here(OYSTER, loc)) { 
                        obj = OYSTER; 
                    } 
                    if (here(CHAIN, loc)) { 
                        if (obj) goto get_object; 
                        obj = CHAIN; 
                    } 
                    if (obj) goto transitive; 
                    puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_WITH_LOCK1 )); 
                    continue; 
                case READ: 
                    obj = read_what(loc); 
                    if (obj != NOTHING) goto transitive; 
                    goto get_object; 
                case INVENTORY: 
                    attempt_inventory(); 
                    continue; 
                case BRIEF: 
                    verbose_interval = 10000; 
                    look_count = 3; 
                    puts(GET_CSTR( CSTR_OKAY_FROM_NOW_ON_LL_ONLY_DESCRIBE_PLACE1 ) SOFT_NL 
                         GET_CSTR( CSTR_YOU_COME_TO_IT_TO_GET_THE_FULL1 )); 
                    continue; 
                case SCORE: 
                    say_score(); 
                    continue; 
                case QUIT: 
                    if (yes(GET_CSTR( CSTR_DO_YOU_REALLY_WANT_TO_QUIT_NOW1 ), ok, ok)) give_up(); 
                    continue; 
#ifdef SAVE_AND_RESTORE 
                case SAVE: 
                    switch (attempt_save()) { 
                        case 0: puts(GET_CSTR( CSTR_SAVE_FAILED )); break; 
                        case 1: puts(GET_CSTR( CSTR_SAVED )); break; 
                        case 2: puts(GET_CSTR( CSTR_RESTORED )); break; 
                    } 
                    continue; 
                case RESTORE: 
                    /* On the fizmo interpreter, @restore yields 2 
                     * when the save file doesn't exist, or when it 
                     * has the wrong serial number for this game. 
                     * I don't know what return value 0 would mean. */ 
                    attempt_restore(); 
                    puts(GET_CSTR( CSTR_RESTORE_FAILED )); 
                    continue; 
#endif /* SAVE_AND_RESTORE */ 
                case FEEFIE: { 
                    static const char *const incantation[] = { GET_CSTR( CSTR_FEE ), GET_CSTR( CSTR_FIE ), GET_CSTR( CSTR_FOE ), GET_CSTR( CSTR_FOO ), GET_CSTR( CSTR_FUM ) }; 
                    int k = 0; 
                    while (!streq(word1, incantation[k])) ++k; 
                    if (foobar == -k) { 
                        foobar = k+1; 
                        if (foobar != 4) { 
                            puts(ok); 
                            continue; 
                        } 
                        foobar = 0; 
                        if (there(EGGS, R_GIANT) || (toting(EGGS) && loc == R_GIANT)) { 
                            puts(GET_CSTR( CSTR_NOTHING_HAPPENS )); 
                            continue; 
                        } 
                        if (there(EGGS, R_LIMBO) && there(TROLL, R_LIMBO) && objs(TROLL).prop == 0) 
                            objs(TROLL).prop = 1;  /* the troll returns */ 
                        if (loc == R_GIANT) { 
                            puts(GET_CSTR( CSTR_THERE_IS_LARGE_NEST_HERE_FULL_OF_GOLDEN3 )); 
                        } else if (here(EGGS, loc)) { 
                            puts(GET_CSTR( CSTR_THE_NEST_OF_GOLDEN_EGGS_HAS_VANISHED1 )); 
                        } else { 
                            puts(GET_CSTR( CSTR_DONE )); 
                        } 
                        move(EGGS, R_GIANT); 
                        continue; 
                    } else if (foobar == 0) { 
                        puts(GET_CSTR( CSTR_NOTHING_HAPPENS )); 
                    } else { 
                        puts(GET_CSTR( CSTR_WHAT_THE_MATTER_CAN_YOU_READ_NOW_YOU1 )); 
                    } 
                    continue; 
                } 
                default: 
                    goto get_object; 
            } 
        transitive: 
            switch (verb) { 
                case SAY: { 
                    if (*word2 != '\0') strcpy(word1, word2); 
                    int k = lookup(word1); 
                    switch (k) { 
                        case XYZZY: case PLUGH: case PLOVER: 
                            mot = k; 
                            goto try_move; 
                        case FEEFIE: 
                            verb = k; 
                            goto intransitive; 
                        default: 
                            puts(GET_CSTR( CSTR_NOT_GOING_TO_SAY_IT1 )); 
                            //printf("Okay, \"%s\".\n", word1); 
                            continue; 
                    } 
                } 
                case EAT: 
                    attempt_eat(obj); 
                    continue; 
                case WAVE: 
                    attempt_wave(obj, loc); 
                    continue; 
                case BLAST: 
                    attempt_blast(loc); 
                    continue; 
                case RUB: 
                    attempt_rub(obj); 
                    continue; 
                case FIND: case INVENTORY: 
                    attempt_find(obj, loc); 
                    continue; 
                case BREAK: 
                    attempt_break(obj, loc); 
                    continue; 
                case WAKE: 
                    attempt_wake(obj); 
                    continue; 
                case ON: 
                    if (!here(LAMP, loc)) { 
                        puts(GET_CSTR( CSTR_YOU_HAVE_NO_SOURCE_OF_LIGHT3 )); 
                        continue; 
                    } 
                    if (lamp_limit < 0) { 
                        puts(GET_CSTR( CSTR_YOUR_LAMP_HAS_RUN_OUT_OF_POWER3 )); 
                        continue; 
                    } 
                    objs(LAMP).prop = 1; 
                    puts(GET_CSTR( CSTR_YOUR_LAMP_IS_NOW_ON )); 
                    if (was_dark) goto commence; 
                    continue; 
                case OFF: 
                    attempt_off(loc); 
                    continue; 
                case DRINK: { 
                    bool stream_here = has_water(loc); 
                    bool evian_here = here(BOTTLE, loc) && (bottle_contents() == WATER); 
                    if (obj == NOTHING) { 
                        if (!stream_here && !evian_here) 
                            goto get_object; 
                    } else if (obj != WATER) { 
                        puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS )); 
                        continue; 
                    } 
                    /* Drink from the bottle if we can; otherwise from the stream. */ 
                    if (evian_here) { 
                        objs(BOTTLE).prop = 1;  /* empty */ 
                        objs(WATER).place = R_LIMBO; 
                        puts(GET_CSTR( CSTR_THE_BOTTLE_OF_WATER_IS_NOW_EMPTY1 )); 
                    } else { 
                        puts(GET_CSTR( CSTR_YOU_HAVE_TAKEN_DRINK_FROM_THE_STREAM1 ) SOFT_NL 
                             GET_CSTR( CSTR_MINERALS_BUT_IS_NOT_UNPLEASANT_IT_IS1 )); 
                    } 
                    continue; 
                } 
                case POUR: 
                    if (obj == NOTHING || obj == BOTTLE) { 
                        obj = bottle_contents(); 
                        if (obj == NOTHING) goto get_object; 
                        /* Notice that POUR BOTTLE when the bottle is empty 
                         * will actually result in the message "Bottle what?". */ 
                    } 
                    if (toting(obj)) { 
                        if (obj != WATER && obj != OIL) { 
                            puts(GET_CSTR( CSTR_YOU_CAN_POUR_THAT )); 
                            continue; 
                        } 
                        objs(BOTTLE).prop = 1;  /* empty */ 
                        objs(obj).place = R_LIMBO; 
                        if (there(PLANT, loc)) { 
                            /* Try to water the plant. */ 
                            if (obj != WATER) { 
                                puts(GET_CSTR( CSTR_THE_PLANT_INDIGNANTLY_SHAKES_THE_OIL1 )); 
                                continue; 
                            } else { 
                                if (objs(PLANT).prop == 0) { 
                                    puts(GET_CSTR( CSTR_THE_PLANT_SPURTS_INTO_FURIOUS_GROWTH1 )); 
                                    objs(PLANT).prop = 1; 
                                } else if (objs(PLANT).prop == 1) { 
                                    puts(GET_CSTR( CSTR_THE_PLANT_GROWS_EXPLOSIVELY_ALMOST1 )); 
                                    objs(PLANT).prop = 2; 
                                } else if (objs(PLANT).prop == 2) { 
                                    puts(GET_CSTR( CSTR_YOU_VE_OVER_WATERED_THE_PLANT_IT1 )); 
                                    objs(PLANT).prop = 0; 
                                } 
                                objs(PLANT2).prop = objs(PLANT).prop; 
                                mot = NOWHERE; 
                                goto try_move; 
                            } 
                        } else if (there(RUSTY_DOOR, loc)) { 
                            /* Pour water or oil on the door. */ 
                            switch (obj) { 
                                case WATER: 
                                    objs(RUSTY_DOOR).prop = 0; 
                                    puts(GET_CSTR( CSTR_THE_HINGES_ARE_QUITE_THOROUGHLY1 )); 
                                    break; 
                                case OIL: 
                                    objs(RUSTY_DOOR).prop = 1; 
                                    puts(GET_CSTR( CSTR_THE_OIL_HAS_FREED_UP_THE_HINGES_SO_THAT1 )); 
                                    break; 
                            } 
                        } else { 
                            puts(GET_CSTR( CSTR_YOUR_BOTTLE_IS_EMPTY_AND_THE_GROUND_IS1 )); 
                        } 
                    } else { 
                        puts(GET_CSTR( CSTR_YOU_AREN_CARRYING_IT8 )); 
                    } 
                    continue; 
                case FILL: 
                    if (attempt_fill(obj, loc)) { 
                        goto get_object; 
                    } 
                    continue; 
                case TAKE: 
                    if (attempt_take(obj, loc)) { 
                        oldverb = TAKE; 
                        verb = FILL; 
                        obj = BOTTLE; 
                        goto transitive; 
                    } 
                    continue; 
                case DROP: 
                    attempt_drop(obj, loc); 
                    continue; 
                case TOSS: 
                    if (obj == ROD && toting(ROD2) && !toting(ROD)) obj = ROD2; 
                    if (!toting(obj)) { 
                        puts(GET_CSTR( CSTR_YOU_AREN_CARRYING_IT9 )); 
                        continue; 
                    } 
                    if (is_treasure(obj) && is_at_loc(TROLL, loc)) { 
                        /* Snarf a treasure for the troll. */ 
                        drop(obj, R_LIMBO); 
                        destroy(TROLL); destroy(TROLL_); 
                        drop(NO_TROLL, R_SWSIDE); drop(NO_TROLL_, R_NESIDE); 
                        juggle(CHASM); juggle(CHASM_); 
                        puts(GET_CSTR( CSTR_THE_TROLL_CATCHES_YOUR_TREASURE_AND1 )); 
                        continue; 
                    } 
                    if (obj == FOOD && here(BEAR, loc)) { 
                        oldverb = TOSS; 
                        verb = FEED; 
                        obj = BEAR; 
                        goto transitive; 
                    } 
                    if (obj != AXE) { 
                        oldverb = TOSS; 
                        verb = DROP; 
                        goto transitive; 
                    } 
                    if (dwarf_at(loc)) { 
                        throw_axe_at_dwarf(loc); 
                    } else if (is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) { 
                        puts(GET_CSTR( CSTR_THE_AXE_BOUNCES_HARMLESSLY_OFF_THE1 )); 
                    } else if (is_at_loc(TROLL, loc)) { 
                        puts(GET_CSTR( CSTR_THE_TROLL_DEFTLY_CATCHES_THE_AXE1 ) SOFT_NL 
                             GET_CSTR( CSTR_BACK_DECLARING_GOOD_WORKMANSHIP_BUT1 )); 
                    } else if (here(BEAR, loc) && objs(BEAR).prop == 0) { 
                        /* Throw the axe at the (un-tame) bear. */ 
                        drop(AXE, loc); 
                        objs(AXE).prop = 1; 
                        immobilize(AXE); 
                        juggle(BEAR);  /* keep bear first in the list */ 
                        puts(GET_CSTR( CSTR_THE_AXE_MISSES_AND_LANDS_NEAR_THE_BEAR1 )); 
                        continue; 
                    } else { 
                        obj = NOTHING; 
                        oldverb = TOSS; 
                        verb = KILL; 
                        goto transitive; 
                    } 
                    drop(AXE, loc); 
                    mot = NOWHERE; 
                    goto try_move; 
                case KILL: 
                    if (obj == NOTHING) { 
                        /* See if there's a unique object to attack. */ 
                        int k = 0; 
                        if (dwarf_at(loc)) { ++k; obj = DWARF; } 
                        if (here(SNAKE, loc)) { ++k; obj = SNAKE; } 
                        if (is_at_loc(DRAGON, loc) && !objs(DRAGON).prop) { ++k; obj = DRAGON; } 
                        if (is_at_loc(TROLL, loc)) { ++k; obj = TROLL; } 
                        if (here(BEAR, loc) && !objs(BEAR).prop) { ++k; obj = BEAR; } 
                        if (k == 0) { 
                            /* no enemies present */ 
                            if (here(BIRD, loc) && oldverb != TOSS) { ++k; obj = BIRD; } 
                            if (here(CLAM, loc) || here(OYSTER, loc)) { ++k; obj = CLAM; } 
                        } 
                        if (k > 1) goto get_object; 
                    } 
                    switch (obj) { 
                        case NOTHING: 
                            puts(GET_CSTR( CSTR_THERE_IS_NOTHING_HERE_TO_ATTACK1 )); 
                            continue; 
                        case BIRD: 
                            if (closed) { 
                                puts(GET_CSTR( CSTR_OH_LEAVE_THE_POOR_UNHAPPY_BIRD_ALONE1 )); 
                            } else { 
                                destroy(BIRD); 
                                objs(BIRD).prop = 0; 
                                if (there(SNAKE, R_HMK)) ++lost_treasures; 
                                puts(GET_CSTR( CSTR_THE_LITTLE_BIRD_IS_NOW_DEAD_ITS_BODY1 )); 
                                continue; 
                            } 
                        case DRAGON: 
                            if (objs(DRAGON).prop) { 
                                puts(GET_CSTR( CSTR_FOR_CRYING_OUT_LOUD_THE_POOR_THING_IS2 )); 
                                continue; 
                            } else { 
                                /* If you insist on attacking the dragon, you win! 
                                 * He dies, the Persian rug becomes free, and R_SCAN2 
                                 * takes the place of R_SCAN1 and R_SCAN3. */ 
                                puts(GET_CSTR( CSTR_WITH_WHAT_YOUR_BARE_HANDS2 )); 
                                verb = NOTHING; 
                                obj = NOTHING; 
                                listen(); 
                                if (streq(word1, GET_CSTR( CSTR_YES )) || streq(word1, GET_CSTR( String1 ))) { 
                                    puts(GET_CSTR( CSTR_CONGRATULATIONS_YOU_HAVE_JUST1 ) SOFT_NL 
                                         GET_CSTR( CSTR_HANDS_UNBELIEVABLE_ISN_IT1 )); 
                                    objs(DRAGON).prop = 1;  /* dead */ 
                                    objs(RUG).prop = 0; 
                                    mobilize(RUG); 
                                    destroy(DRAGON_); 
                                    destroy(RUG_); 
                                    for (int t = MIN_OBJ; t <= MAX_OBJ; ++t) { 
                                        if (there(t, R_SCAN1) || there(t, R_SCAN3)) 
                                            move(t, R_SCAN2); 
                                    } 
                                    loc = R_SCAN2; 
                                    mot = NOWHERE; 
                                    goto try_move; 
                                } else { 
                                    goto pre_parse; 
                                } 
                            } 
                        case CLAM: 
                        case OYSTER: 
                            puts(GET_CSTR( CSTR_THE_SHELL_IS_VERY_STRONG_AND1 )); 
                            continue; 
                        case SNAKE: 
                            puts(GET_CSTR( CSTR_ATTACKING_THE_SNAKE_BOTH_DOESN_WORK1 )); 
                            continue; 
                        case DWARF: 
                            if (closed) dwarves_upset(); 
                            puts(GET_CSTR( CSTR_WITH_WHAT_YOUR_BARE_HANDS3 )); 
                            continue; 
                        case TROLL: 
                            puts(GET_CSTR( CSTR_TROLLS_ARE_CLOSE_RELATIVES_WITH_THE1 ) SOFT_NL 
                                 GET_CSTR( CSTR_RHINOCEROS_HIDE_THE_TROLL_FENDS_OFF1 )); 
                            continue; 
                        case BEAR: 
                            switch (objs(BEAR).prop) { 
                                case 0: puts(GET_CSTR( CSTR_WITH_WHAT_YOUR_BARE_HANDS_AGAINST_HIS1 )); break; 
                                case 3: puts(GET_CSTR( CSTR_FOR_CRYING_OUT_LOUD_THE_POOR_THING_IS3 )); break; 
                                default: puts(GET_CSTR( CSTR_THE_BEAR_IS_CONFUSED_HE_ONLY_WANTS_TO_BE1 )); break; 
                            } 
                            continue; 
                        default: 
                            puts(GET_CSTR( CSTR_DON_BE_RIDICULOUS )); 
                            continue; 
                    } 
                case FEED: 
                    attempt_feed(obj, loc); 
                    continue; 
                case OPEN: case CLOSE: 
                    attempt_open_or_close(verb, obj, loc); 
                    continue; 
                case READ: 
                    if (now_in_darkness(loc)) goto cant_see_it; 
                    attempt_read(obj); 
                    continue; 
                case CALM: 
                    puts(GET_CSTR( CSTR_GAME_WOULD_YOU_CARE_TO_EXPLAIN_HOW3 )); 
                    continue; 
                case GO: 
                    puts(GET_CSTR( CSTR_WHERE )); 
                    continue; 
                case RELAX: 
                    puts(ok);  /* this corresponds to the command "NOTHING LAMP" */ 
                    continue; 
                case FEEFIE: 
                    puts(GET_CSTR( CSTR_DON_KNOW_HOW ));  /* "FOO BARS" */ 
                    continue; 
                case BRIEF: 
                    puts(GET_CSTR( CSTR_ON_WHAT ));  /* har har har */ 
                    continue; 
                case SCORE: 
                case QUIT: 
#ifdef SAVE_AND_RESTORE 
                case SAVE: 
                case RESTORE: 
#endif /* SAVE_AND_RESTORE */ 
                    puts(GET_CSTR( CSTR_EH )); 
                    continue; 
            } 
        get_object: 
            word1[0] = toupper(word1[0]); 
            puts(word1); 
            puts(GET_CSTR( CSTR_WHAT1 )); 
            goto cycle; 
        cant_see_it: 
            if ((verb == FIND || verb == INVENTORY) && *word2 != '\0') 
                goto transitive; 
            puts_nocr(GET_CSTR( CSTR_SEE_NO )); 
            puts_nocr( word1 ); 
            puts_nocr(GET_CSTR( CSTR_HERE )); 
            continue; 
        } 
    try_move: 
        /* A major cycle comes to an end when a motion verb mot has been 
         * given and we have computed the appropriate newloc accordingly. */ 
        assert(R_LIMBO <= oldloc && oldloc <= MAX_LOC); 
        assert(R_LIMBO < loc && loc <= MAX_LOC); 
        newloc = loc;  /* by default we will stay put */ 
 
        if (mot == CAVE) { 
            /* No CAVE appears in the travel table; its sole purpose is to 
             * give these messages. */ 
            if (loc < MIN_IN_CAVE) { 
                puts(GET_CSTR( CSTR_CAN_SEE_WHERE_THE_CAVE_IS_BUT1 ) SOFT_NL 
                     GET_CSTR( CSTR_THE_SURFACE_FOR_LONG_WOULD_TRY_THE1 )); 
            } else { 
                puts(GET_CSTR( CSTR_NEED_MORE_DETAILED_INSTRUCTIONS_TO_DO1 )); 
            } 
        } else if (mot == LOOK) { 
            /* Repeat the long description and continue. */ 
            if (++look_count <= 3) { 
                puts(GET_CSTR( CSTR_SORRY_BUT_AM_NOT_ALLOWED_TO_GIVE_MORE1 ) SOFT_NL 
                     GET_CSTR( CSTR_LONG_DESCRIPTION_OF_YOUR_LOCATION1 )); 
            } 
            was_dark = false;  /* pretend it wasn't dark, so you won't fall into a pit */ 
            places[loc].visits = 0; 
        } else { 
            if (mot == BACK) { 
                Location l = (is_forced(oldloc) ? oldoldloc : oldloc); 
                mot = try_going_back_to(l, loc); /* may return NOWHERE */ 
            } 
 
            if (mot != NOWHERE) { 
                /* Determine the next newloc. */ 
                oldoldloc = oldloc; 
                oldloc = loc; 
                if (determine_next_newloc(loc, &newloc, mot, verb)) { 
                    /* Player died trying to cross the troll bridge. */ 
                    oldoldloc = newloc;  /* if you are revived, you got across */ 
                    goto death; 
                } 
            } 
        } 
    } 
  pitch_dark: 
    puts(GET_CSTR( CSTR_YOU_FELL_INTO_PIT_AND_BROKE_EVERY_BONE1 )); 
    oldoldloc = loc; 
  death: 
    /* When you die, newloc is undefined (often R_LIMBO) and oldloc is what 
     * killed you. So we look at oldoldloc, the last place you were safe. */ 
    kill_the_player(oldoldloc); 
    loc = oldloc = R_HOUSE; 
    goto commence; 
} 
 
void dwarves_upset(void) 
{ 
    puts(GET_CSTR( CSTR_THE_RESULTING_RUCKUS_HAS_AWAKENED_THE1 ) SOFT_NL 
         GET_CSTR( CSTR_THREATENING_LITTLE_DWARVES_IN_THE1 ) SOFT_NL 
         GET_CSTR( CSTR_KNIVES_AT_YOU_ALL_OF_THEM_GET_YOU1 )); 
    quit(); 
} 
 
int main() 
{ 
    offer(0); 
    /* Reading the instructions marks you as a newbie who will need the 
     * extra lamp power. However, it will also cost you 5 points. */ 
    lamp_limit = (hints[0].given ? 1000 : 330); 
    build_travel_table(); 
    build_object_table(); 
    simulate_an_adventure(); 
    return 0; 
}