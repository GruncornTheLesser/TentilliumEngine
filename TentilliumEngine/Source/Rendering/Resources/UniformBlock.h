#pragma once
#include <array>
// size is 4, 8 or 16 bytes
// bool		size = 4	align = 4 bytes (bool in machine code are 1 byte)
// float	size = 4	align = 4 bytes
// vec2		size = 8	align = 8 bytes
// vec3		size = 16	align = 16 bytes
// vec4		size = 16	align = 16 bytes
// array<T, N> = T * N
// mat3 = array<vec3, 3>
// mat4 = array<vec4, 4>



namespace internal {
	template<typename ... Ts>
	struct uniformBlock {
	private:
		static inline size_t alignment(size_t size) {
			if (size <= 4) return 4;
			if (size <= 8) return 8;
			return 16;
		}

		static inline void realign(size_t& offset, size_t alignment) {
			offset = (offset % alignment == 0) ? offset : offset + alignment - (offset % alignment);
		}

		static inline bool next(size_t& offset, size_t size, size_t align, unsigned int count, unsigned int& index, unsigned int limit) {
			realign(offset, align);				// realigns offset to the alignment
		
			if (index == limit) return true;	// stop loop, make limit = -1 to loop through all
		
			offset += size;						// add size of new value to size
			index += 1;							// iterate to next
		
			return false;						// continue loop
		}

		static inline size_t calcOffset(unsigned int limit) {
			size_t offset = 0;
			unsigned int i = 0;
			
			(next(offset, sizeof(Ts), alignment(sizeof(Ts)), /*std::is_array_v<Ts>? : */1, i, limit) || ...);
			return offset;
		}

	public:
		const static inline size_t size = calcOffset(-1);

		template<unsigned int i>
		const static inline size_t offset = calcOffset(i);

		uniformBlock() = delete;
	};
}