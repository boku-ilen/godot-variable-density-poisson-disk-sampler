@tool
extends Node


@export_tool_button("Generate") var generate_button = generate


func generate():
	var sampler = VariablePoissonSampler2D.new();
	
	var icon = preload("res://icon.svg")
	var density_func = func(x, y):
		return lerp(1.5, 0.5, icon.get_image().get_pixel(x, y).b)
	
	var time_before = Time.get_ticks_msec()
	sampler.generate(density_func, 1., 10., 127.0, 127.0, 1.0, 30);
	var time_after = Time.get_ticks_msec()
	
	print("Generating %s points took %s msec" % [str(sampler.get_samples().size()), str(time_after - time_before)])
	
	$MultiMeshInstance3D.multimesh.instance_count = sampler.get_samples().size()
	
	var mm_index = 0
	for sample in sampler.get_samples():
		var position = Vector3(sample.x, 0.0, sample.y)
		
		$MultiMeshInstance3D.multimesh.set_instance_transform(mm_index, Transform3D().translated(position))
		
		mm_index += 1
