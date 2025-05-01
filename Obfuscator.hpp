/*
	Source Code Creation Date
		 20:25 	 05:01 		
	MIT Open Source License
		Dev: Metzkerson
		QQ: 3594143112
*/

//	对于开源精神和开源理解，此程序请不要删除原开源作者所属名称
//	五一快乐！
//	可以反GG修改器内存模糊和精确搜索
//	篡改验证 如果被非法更改验证失败 return -1
//	我相信开源能让这个世界变得更加美好！
//	这个是我的简写缩写版，实际上我自己的是非常庞大的一个反修改器的装置
//	为了更好的理解，我将它缩写成了一个有基本保护的源代码
//	希望能帮助到你们！
//	记得每天保持微笑，我相信明天会更好！

#pragma once

// 定义
#define ATOMIC_INVALID -1 // 校验失败
#define ATOMIC_TRUE	1 // 真值
#define ATOMIC_FALSE	0 // 假值

// 常量 更改可以自定义解析
#define STATIC_SALT1 0x9E3779B9
#define STATIC_SALT2 0x6A09E667

//	解释单元 支持C/C++
typedef struct {
	union {
		struct {
			uint8_t enc_value;
			uint32_t base_sig;
			uint32_t op_sig;
		};
		uint8_t raw[12];
	} data;
	uint32_t dyn_key;
	uint32_t create_time;
} AtomicInt;

static uint32_t gen_base_sig(uint8_t enc_val, uint32_t key, uint32_t time) {
	return (enc_val * 0x01000193) ^ key ^ STATIC_SALT1 ^ time;
}

static uint32_t gen_op_sig(uint32_t base, uint32_t key) {
	return (base >> 16 | base << 16) ^ key ^ STATIC_SALT2;
}

static uint8_t enc_val(int val, uint32_t key) {
	return (val == ATOMIC_TRUE ? 0xAA : 0x55) ^ (key & 0xFF);
}

static int dec_val(uint8_t enc, uint32_t key) {
	return ((enc ^ (key & 0xFF)) == 0xAA) ? ATOMIC_TRUE : ATOMIC_FALSE;
}

static bool AtomicInt_valid(const AtomicInt* sai) {
	uint32_t expect_base = gen_base_sig(sai->data.enc_value, sai->dyn_key, sai->create_time);
	uint32_t expect_op = gen_op_sig(expect_base, sai->dyn_key);
	return sai->data.base_sig == expect_base && sai->data.op_sig == expect_op;
}

static int AtomicInt_val(const AtomicInt* sai) {
	return AtomicInt_valid(sai) ? dec_val(sai->data.enc_value, sai->dyn_key) : ATOMIC_INVALID;
}

static int AtomicInt_not(const AtomicInt* sai) {
	int val = AtomicInt_val(sai);
	return (val != ATOMIC_INVALID) ? !val : ATOMIC_INVALID;
}

void AtomicInt_init(AtomicInt* sai, int init_val) {
	sai->create_time = (uint32_t)time(NULL);
	sai->dyn_key = (sai->create_time * 0x85EBCA77) ^ STATIC_SALT1;
	
	sai->data.enc_value = enc_val(init_val, sai->dyn_key);
	sai->data.base_sig = gen_base_sig(sai->data.enc_value, sai->dyn_key, sai->create_time);
	sai->data.op_sig = gen_op_sig(sai->data.base_sig, sai->dyn_key);
}

int AtomicInt_load(const AtomicInt* sai) {
	return AtomicInt_val(sai);
}

void AtomicInt_store(AtomicInt* sai, int new_val) {
	sai->dyn_key = (sai->dyn_key * 0x85EBCA77 + 1) ^ STATIC_SALT2;
	sai->data.enc_value = enc_val(new_val, sai->dyn_key);
	sai->data.base_sig = gen_base_sig(sai->data.enc_value, sai->dyn_key, sai->create_time);
	sai->data.op_sig = gen_op_sig(sai->data.base_sig, sai->dyn_key);
}