-module(porttest).

-compile(export_all).

-define(CMD_NAME, "./cport").

test_suite_new(PortNum, BlockSize) ->
    Block = <<0:(BlockSize * 8)>>,
    Pids = [test_port_new(Block) || _ <- lists:seq(1, PortNum)],
    {ok, Pids}.

run_test_suite(Pids, Count) ->
    io:format("*** test started ***~n      "),
    lists:foreach(
      fun(X) ->
	      io:format("#~-6B ", [X])
      end,
      lists:seq(1, length(Pids))),
    io:format("~n"),
    run_test_suite(Pids, Count, 0).

run_test_suite(_, Count, Count) ->
    io:format("*** test end ***~n"),
    ok;
run_test_suite(Pids, Count, Current) ->
    Keys = [rpc:async_call(node(), ?MODULE, test_once, [Pid]) || Pid <- Pids],
    io:format("~4B: ", [Current + 1]),
    lists:foreach(
      fun(XKey) ->
	      {value, {ok, XValue}} = rpc:nb_yield(XKey, 1000),
	      io:format("~-7B ", [XValue])
      end,
      Keys),
    io:format("~n"),
    run_test_suite(Pids, Count, Current + 1).

test_once(Pid) ->
    Pid ! {test, self()},
    receive
	{ok, Pid, Result} ->
	    {ok, Result}

    after 1000 ->
	    exit(timeout)
    end.

test_port_new(Block) ->
    spawn_link(?MODULE, port_loop, [Block]).

port_loop(Block) ->
    PortId = open_port({spawn, ?CMD_NAME}, [use_stdio, binary, {packet, 4}]),
    port_loop(PortId, Block).

port_loop(PortId, Block) ->
    receive
	{test, Caller} ->
	    StartTime = now(),
	    true = port_command(PortId, [Block]),

	    receive
		{PortId, {data, Block}} ->
		    Caller ! {ok, self(), timer:now_diff(now(), StartTime)},
		    port_loop(PortId, Block);

		X ->
		    exit({port_loop_badarg, X})

	    after 1000 ->
		    exit(port_timeout)
	    end;

	X ->
	    exit({port_loop_unknown_message, X})
    end.
