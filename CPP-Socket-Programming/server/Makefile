all:
	g++ -std=c++0x -I /media/data/git/orchid-sw/orchid/thirdparty/protobuf/protobuf-2.4.1/include/ server.cc License.pb.cc -L=/media/data/git/orchid-sw/orchid/thirdparty/protobuf/protobuf-2.4.1/lib/ -lprotobuf -pthread -o server

protobuild:
	protoc -I=. --cpp_out=. License.proto

reader:
	g++ -std=c++0x -I /media/data/git/orchid-sw/orchid/thirdparty/protobuf/protobuf-2.4.1/include/ deserializer.cc License.pb.cc -L=/media/data/git/orchid-sw/orchid/thirdparty/protobuf/protobuf-2.4.1/lib/ -lprotobuf -pthread -o deserializer
