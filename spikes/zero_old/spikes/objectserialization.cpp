#include <deque>
#include <iostream>
#include <string.h>
#include <tuple>
#include <vector>
#include <deque>

typedef void (*readfunc)();
std::vector<readfunc> typemappings;

class Buffer
{
public:
	Buffer()
	: writeoff(0)
	, readoff(0)
	{
	}
	unsigned char buf[1024];
	size_t writeoff;
	size_t readoff;
};

Buffer buf;

template <typename T>
static void write(T data)
{
	memcpy(&buf.buf[buf.writeoff], &T::id, sizeof(T::id));
	buf.writeoff += sizeof(T::id);
	memcpy(&buf.buf[buf.writeoff], &data, sizeof(data));
	buf.writeoff += sizeof(data);
}

static void read()
{
	uint32_t id;
	memcpy(&id, &buf.buf[buf.readoff], sizeof(id));
	buf.readoff += sizeof(id);
	typemappings[id]();
}


template <typename T>
class Packet
{
public:
	static std::deque<T> packets;
	static uint32_t id;
	static void read()
	{
		T packet;
		memcpy(&packet, &buf.buf[buf.readoff], sizeof(T));
		buf.readoff += sizeof(T);	
		packets.push_back(packet);
	}
};
template <typename T>
uint32_t Packet<T>::id;
template <typename T>
std::deque<T> Packet<T>::packets;

static uint32_t id = 0;

template <typename T>
void RegisterPacket()
{
	uint32_t currentid = id++;
	T::id = currentid;
	typemappings.push_back(T::read);
}

class HealthPacket : public Packet<HealthPacket>
{
public:
	int32_t health;
};

class BulletPacket : public Packet<BulletPacket>

{
public:
	int32_t speed;
};


int main()
{
	RegisterPacket<BulletPacket>();
	RegisterPacket<HealthPacket>();

	HealthPacket health;	
	BulletPacket bullet;
	bullet.speed = 42;
	health.health = 9001;
	write(bullet);
	write(health);
	read();
	read();
	std::cout << "Bullet speed should be 42: " << BulletPacket::packets.front().speed << std::endl;
	std::cout << "Health should be over 9000: " << HealthPacket::packets.front().health << std::endl;
	return 0;
}
