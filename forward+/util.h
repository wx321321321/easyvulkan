#pragma once


template <class T>
void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 tex_coord;
	glm::vec3 normal;

	using index_t = uint32_t;

	bool operator==(const Vertex& other) const noexcept
	{
		return pos == other.pos && color == other.color && tex_coord == other.tex_coord && normal == other.normal;
	}

	size_t hash() const
	{
		size_t seed = 0;
		hash_combine(seed, pos);
		hash_combine(seed, color);
		hash_combine(seed, tex_coord);
		hash_combine(seed, normal);
		return seed;
	}
};
