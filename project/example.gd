@tool
extends Node


@export_tool_button("Generate") var generate_button = generate


func generate():
	# Clear previous
	for child in get_children():
		if child is MeshInstance3D: child.queue_free()
	
	var sampler = VariablePoissonSampler2D.new();
	
	var time_before = Time.get_ticks_msec()
	sampler.generate(1.0, 100.0, 100.0, 30);
	var time_after = Time.get_ticks_msec()
	
	print("Generating %s points took %s msec" % [str(sampler.get_samples().size()), str(time_after - time_before)])
	
	for sample in sampler.get_samples():
		var position = Vector3(sample.x, 0.0, sample.y)
		
		var node = preload("res://marker.tscn").instantiate()
		node.position = position
		add_child(node)
