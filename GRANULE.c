/*
 * GRANULE.c
 * 
 * Copyright 2019 zakhar343 <zakhar343@zakhar343-HP-ENVY-dv7-Notebook-PC>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#define NUM_ROUNDS 32
uint8_t SUBSTITUTION[16] = {14, 7, 8, 4, 1, 9, 2, 15, 5, 10, 11, 0, 6, 12, 13, 3};
uint8_t PERMUTATION[8] = {4, 0, 3, 1, 6, 2, 7, 5};

uint32_t get_PT0(uint64_t PT){
	return (uint32_t)PT;
};

uint32_t get_PT1(uint64_t PT){
	return PT >> 32;
};

uint64_t get_PT(uint32_t PT1, uint32_t PT0){
	uint64_t PT=0;
	PT ^= PT1;
	PT <<= 32;
	PT ^= PT0;
	return PT;
};

uint64_t gen_key(uint64_t *key, uint8_t round){
	uint64_t temp, sub_bit;
	temp = key[1] >> 33;
	key[1] <<= 31;
	key[1] |= key[0] >> 33;
	key[0] <<= 31;
	key[0] |= temp;
	sub_bit = key[0] & 15;
	sub_bit = SUBSTITUTION[(int)sub_bit];
	key[0] = key[0] & (~15);
	key[0] |= sub_bit;
	sub_bit = (key[0] & 240) >> 4;
	sub_bit = SUBSTITUTION[(int)sub_bit];
	key[0] = key[0] & (~240);
	key[0] |= (sub_bit << 4);
	key[1] ^= (round << 2);
	return (key[0] << 32);
	};

uint64_t F_PT(uint64_t PT){
	uint32_t PT1 = get_PT1(PT), PT0 = get_PT0(PT), SP_PT=0;
	uint8_t PT_init[8], PT_perm[8], PT_sub[8];
	for (int i = 0; i < 8; i++){
		PT_init[i] = (PT1 >> (4*i)) & 15;
	};
	for (int i = 0; i < 8; i++){
		PT_perm[(int)PERMUTATION[i]] = PT_init[i];
	};
	for (int i = 0; i < 8; i++){
		PT_sub[i] = SUBSTITUTION[(int)PT_perm[i]];
	};
	SP_PT |= PT_perm[7];
	for (int i = 7; i > -1; i--){
		SP_PT <<= 4;
		SP_PT |= PT_sub[i];
	};
	PT0 = ((SP_PT << 2) | (SP_PT >> 30)) ^ ((SP_PT >> 7) | (SP_PT << 25)) ^ PT0;
	PT = get_PT(PT0, PT1);
	return PT;
};

int main(int argc, char **argv){
	uint64_t *key;
	key = malloc(2*sizeof(uint64_t));
	key[0] = 0xfedcba9876543210;
	key[1] = 0x0123456789abcdee;
	uint64_t PT = 0xabdf098723dfeba;
	uint64_t rk = (key[0] & 0xffffffff) << 32;
	for (int i = 0; i < NUM_ROUNDS; i++){
		PT = F_PT(PT);
		PT ^= rk;
		rk = gen_key(key, i);
	};
	printf("%lx", PT);
	free(key);
	return 0;
}

