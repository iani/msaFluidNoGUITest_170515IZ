
//: Testing random points
{
	var addr;
	addr = NetAddr ("127.0.0.1", 12345);
	(1 .. 79000) do: { | index |
		index.postln;
		addr.sendMsg ('/vertex', 900.0.rand, 800.0.rand, 1200.0.rand.neg, 1.0.rand, 1.0.rand, 1.0.rand);
		0.1.wait;
	}
}.fork

//: Testing points within a range to see how cam and translation works
{
	var addr;
	addr = NetAddr ("127.0.0.1", 57130);
	(0..1000) do: { | x |
		x = x * 10;
		// x.postln;
		addr.sendMsg ('/data', x * 1.0, x * 1.0 + 100, x.neg * 1.0, 1.0.rand, 1.0.rand, 1.0.rand);
		0.1.wait;
	}
}.fork
//:

