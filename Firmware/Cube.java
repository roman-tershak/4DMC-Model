package com.acxiom.dmp;

import java.util.Arrays;


public class Cube {

    private static final int MAX_COLOR = 50;
    
    private static int ct = 0, state = 1, mode = 1;
    private static int[] cts = new int[] {0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    public static void main(String[] args) {
        isr(30, 0);
//        isr(2);
//        System.err.println(Arrays.toString(cts));
//        testDistribution();
    }

    private static void testDistribution() throws Error {
        for (int i = 0; i <= MAX_COLOR; i++) {
            isr(i);
            for (int j = 0; j < 9; j++) {
                if (cts[j] != i) {
                    System.err.println(i);
                    System.err.println(Arrays.toString(cts));
                    
                    throw new Error();
                }
            }
            Arrays.fill(cts, 0);
        }
    }

    private static void isr(int color) {
        while (true) {
            if (mode == 1) {
                if (color >= state) {
                    set_pin(true);
                } else {
                    set_pin(false);
                }
                mode = 0;
            } else {
                ct++; //increase the row
                if (ct > 8) {
                    ct = 0; //reset
                    // System.out.println("----------------");
                }

                state += 3;
                if (state > MAX_COLOR) {
                    state -= MAX_COLOR; //compare color overflow
                    // System.out.println("------------------------------------------------");
                }
                if (state == 0) throw new Error();

                mode = 1;
            }
            
            if (ct == 0 && state == 1 && mode == 1) 
                break;
        }
    }

    private static void isr(int color, int pin) {
        while (true) {
            if (mode == 1) {
                if (color >= state && (pin == ct || pin == -1)) {
                    set_pin(true);
                } else {
                    set_pin(false);
                }
                mode = 0;
            } else {
                ct++; //increase the row
                if (ct > 8) {
                    ct = 0; //reset
                    // System.out.println("----------------");
                }
                
                state += 3;
                if (state > MAX_COLOR) {
                    state -= MAX_COLOR; //compare color overflow
                    // System.out.println("------------------------------------------------");
                }
                if (state == 0) throw new Error();
                
                mode = 1;
            }
            
            if (ct == 0 && state == 1 && mode == 1) 
                break;
        }
    }
    
    private static void set_pin(boolean on) {
        if (on) {
            cts[ct]++;
//            System.out.println(state + "\t" + ct + " - @@@@@@@");
            System.out.print(state + "[" + ct + "]@@\t");
        } else {
            System.out.print(state + "[" + ct + "]\t");
        }
        if (state >= 48) {
            System.out.println();
        }
    }
    
}
