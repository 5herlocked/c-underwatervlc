### A Pluto.jl notebook ###
# v0.15.1

using Markdown
using InteractiveUtils

# This Pluto notebook uses @bind for interactivity. When running this notebook outside of Pluto, the following 'mock version' of @bind gives bound variables a default value (instead of an error).
macro bind(def, element)
    quote
        local el = $(esc(element))
        global $(esc(def)) = Core.applicable(Base.get, el) ? Base.get(el) : missing
        el
    end
end

# ╔═╡ 72def700-f655-11eb-3347-0f460332501c
begin
	using PlutoUI
	using CSV
end

# ╔═╡ 40e2df25-4309-4d8f-94d9-1f19f08decee
md"# Block-By-Block Checking"

# ╔═╡ e4796ed5-3edc-4e13-855f-ce98f9507b00
md"Imports"

# ╔═╡ 43a2f15e-0f74-417f-9457-e285610a2a64
md"## Initial Setup"

# ╔═╡ c210e2e6-d985-4177-b516-82a158aaf505
md"""Ratio of Transmitter to Receiver: $(@bind ratio NumberField(2:100; default=4))"""

# ╔═╡ 7ca8cc67-7da5-4a97-a2e2-ebb1314a999a
md"""Size of window: $(@bind window_size NumberField(1:ratio; default=0))"""

# ╔═╡ 82368edb-f787-4e74-9c42-baca1198b800
md"## Analysing block by block"

# ╔═╡ 9deb7f76-be16-4030-92bc-b7c6cde58fc1
md"## Summary"

# ╔═╡ 2131a8a4-b0c0-49e5-ac7f-0e9977e39366
md"""
The initial approach to transposing the bits logged by the transmitter and those received was to follow a windowing approach to the entire problem.

Take a window the size of the difference between the transmitter and receiver[1] and then count the number of correct and incorrect bits within the window. Logically[2], it makes complete sense after we find the hard start of the transmission[3], to just slowly slide across the entire received section and compare the bits.

The reason for such high BER's is that anytime a bit is dropped/recorded a spare time, there's a padded bit that's introdued leading to an offset within the receiver logs. The problem is compounded by the windowing method where at any given state we're looking at a very small subset of the data. A lot of these errors will be fixed when we switch to something that resembles a perfect media because there will be fewer to no "offset" bits which means the window will almost always be correct. The second way I feel this can be corrected on the transposition side is through the use of convolution which will let us smooth out a lot of these smaller irregularities while looking at each received transmission hollistically as a function rather than discrete data points.

[1] in all of these cases 4

[2] Assuming the receiver is a perfect media.

[3] which we do very effecitvely with the current approach"""

# ╔═╡ 7b5d9c9f-89aa-4e1e-9c47-7b77fa12b0ad
md"#### Helper functions"

# ╔═╡ f2a8a94b-4b39-48be-93c4-fa7661d03014
function getTransmitterPattern(transmitterVector::Vector{Int16})::String
	pattern = ""
	
	for i in range(1, 5; step=1)
		bit = transmitterVector[i]
		for repeat in range(1, ratio; step=1)
			pattern *= string(bit)
		end
	end
	
	return pattern
end

# ╔═╡ 1f993f0e-eb44-4bf6-91b7-971bb133974c
function getReceiverString(receiverVector::Vector{Int16})::String
	receiver = ""
	
	for i in range(1, 2000; step=1)
		receiver *= string(receiverVector[i])
	end
	
	return receiver
end

# ╔═╡ 5506f3db-89f0-40d6-a521-8f7818f866fc
function getVectorFromFile(file::CSV.File)::Vector{Int16}
	fileVector = Vector{Int16}()
	
	for log in file
		push!(fileVector, log.bit)
	end
	
	return fileVector
end

# ╔═╡ 3da9ea6a-d89b-49a6-83cc-132a376a0f00
begin
	transmitter_url = ".\\test-set\\transmitter_25Hz_40c.csv"
	transmitter_file = CSV.File(transmitter_url)
	transmitter_vec = getVectorFromFile(transmitter_file)
end

# ╔═╡ 344f30b1-efae-4286-8d95-665d65a0d6cb
md"""Starting point: $(@bind start_point NumberField(1:size(transmitter_vec)[1]-ratio; default=1))"""

# ╔═╡ f602abe8-db66-46c0-b5d4-42faa2b872aa
transmitter_slice = transmitter_vec[start_point:start_point+window_size]

# ╔═╡ 83219521-6ee7-40b5-abd5-057ca98491fa
begin
	receiver_url = ".\\test-set\\25hz_100fps_40c.csv"
	receiver_file = CSV.File(receiver_url)
	receiver_vec = getVectorFromFile(receiver_file)
end

# ╔═╡ 3709c25a-d5f6-470d-9d76-f6019dda570c
transmission_start = findfirst(getTransmitterPattern(transmitter_vec), getReceiverString(receiver_vec))[1]

# ╔═╡ cd9c5c29-407c-4349-a1d5-45616936bfcf
rec_temp_start = transmission_start + ((start_point - 1) * ratio)

# ╔═╡ 59a7d05b-e988-4ff2-93d5-8155e92bb2b9
rec_temp_end = rec_temp_start + ((size(transmitter_slice)[1]) * ratio) - 1

# ╔═╡ 0f3ef489-767c-425d-870d-15c0a06ab3c0
receiver_slice = receiver_vec[rec_temp_start:rec_temp_end]

# ╔═╡ 00000000-0000-0000-0000-000000000001
PLUTO_PROJECT_TOML_CONTENTS = """
[deps]
CSV = "336ed68f-0bac-5ca0-87d4-7b16caf5d00b"
PlutoUI = "7f904dfe-b85e-4ff6-b463-dae2292396a8"

[compat]
CSV = "~0.8.5"
PlutoUI = "~0.7.9"
"""

# ╔═╡ 00000000-0000-0000-0000-000000000002
PLUTO_MANIFEST_TOML_CONTENTS = """
# This file is machine-generated - editing it directly is not advised

[[Base64]]
uuid = "2a0f44e3-6c83-55bd-87e4-b1978d98bd5f"

[[CSV]]
deps = ["Dates", "Mmap", "Parsers", "PooledArrays", "SentinelArrays", "Tables", "Unicode"]
git-tree-sha1 = "b83aa3f513be680454437a0eee21001607e5d983"
uuid = "336ed68f-0bac-5ca0-87d4-7b16caf5d00b"
version = "0.8.5"

[[DataAPI]]
git-tree-sha1 = "ee400abb2298bd13bfc3df1c412ed228061a2385"
uuid = "9a962f9c-6df0-11e9-0e5d-c546b8b5ee8a"
version = "1.7.0"

[[DataValueInterfaces]]
git-tree-sha1 = "bfc1187b79289637fa0ef6d4436ebdfe6905cbd6"
uuid = "e2d170a0-9d28-54be-80f0-106bbe20a464"
version = "1.0.0"

[[Dates]]
deps = ["Printf"]
uuid = "ade2ca70-3891-5945-98fb-dc099432e06a"

[[Future]]
deps = ["Random"]
uuid = "9fa8497b-333b-5362-9e8d-4d0656e87820"

[[InteractiveUtils]]
deps = ["Markdown"]
uuid = "b77e0a4c-d291-57a0-90e8-8db25a27a240"

[[IteratorInterfaceExtensions]]
git-tree-sha1 = "a3f24677c21f5bbe9d2a714f95dcd58337fb2856"
uuid = "82899510-4779-5014-852e-03e436cf321d"
version = "1.0.0"

[[JSON]]
deps = ["Dates", "Mmap", "Parsers", "Unicode"]
git-tree-sha1 = "8076680b162ada2a031f707ac7b4953e30667a37"
uuid = "682c06a0-de6a-54ab-a142-c8b1cf79cde6"
version = "0.21.2"

[[Libdl]]
uuid = "8f399da3-3557-5675-b5ff-fb832c97cbdb"

[[LinearAlgebra]]
deps = ["Libdl"]
uuid = "37e2e46d-f89d-539d-b4ee-838fcccc9c8e"

[[Logging]]
uuid = "56ddb016-857b-54e1-b83d-db4d58db5568"

[[Markdown]]
deps = ["Base64"]
uuid = "d6f4376e-aef5-505a-96c1-9c027394607a"

[[Mmap]]
uuid = "a63ad114-7e13-5084-954f-fe012c677804"

[[Parsers]]
deps = ["Dates"]
git-tree-sha1 = "bfd7d8c7fd87f04543810d9cbd3995972236ba1b"
uuid = "69de0a69-1ddd-5017-9359-2bf0b02dc9f0"
version = "1.1.2"

[[PlutoUI]]
deps = ["Base64", "Dates", "InteractiveUtils", "JSON", "Logging", "Markdown", "Random", "Reexport", "Suppressor"]
git-tree-sha1 = "44e225d5837e2a2345e69a1d1e01ac2443ff9fcb"
uuid = "7f904dfe-b85e-4ff6-b463-dae2292396a8"
version = "0.7.9"

[[PooledArrays]]
deps = ["DataAPI", "Future"]
git-tree-sha1 = "cde4ce9d6f33219465b55162811d8de8139c0414"
uuid = "2dfb63ee-cc39-5dd5-95bd-886bf059d720"
version = "1.2.1"

[[Printf]]
deps = ["Unicode"]
uuid = "de0858da-6303-5e67-8744-51eddeeeb8d7"

[[Random]]
deps = ["Serialization"]
uuid = "9a3f8284-a2c9-5f02-9a11-845980a1fd5c"

[[Reexport]]
git-tree-sha1 = "5f6c21241f0f655da3952fd60aa18477cf96c220"
uuid = "189a3867-3050-52da-a836-e630ba90ab69"
version = "1.1.0"

[[SentinelArrays]]
deps = ["Dates", "Random"]
git-tree-sha1 = "a3a337914a035b2d59c9cbe7f1a38aaba1265b02"
uuid = "91c51154-3ec4-41a3-a24f-3f23e20d615c"
version = "1.3.6"

[[Serialization]]
uuid = "9e88b42a-f829-5b0c-bbe9-9e923198166b"

[[Suppressor]]
git-tree-sha1 = "a819d77f31f83e5792a76081eee1ea6342ab8787"
uuid = "fd094767-a336-5f1f-9728-57cf17d0bbfb"
version = "0.2.0"

[[TableTraits]]
deps = ["IteratorInterfaceExtensions"]
git-tree-sha1 = "c06b2f539df1c6efa794486abfb6ed2022561a39"
uuid = "3783bdb8-4a98-5b6b-af9a-565f29a5fe9c"
version = "1.0.1"

[[Tables]]
deps = ["DataAPI", "DataValueInterfaces", "IteratorInterfaceExtensions", "LinearAlgebra", "TableTraits", "Test"]
git-tree-sha1 = "d0c690d37c73aeb5ca063056283fde5585a41710"
uuid = "bd369af6-aec1-5ad0-b16a-f7cc5008161c"
version = "1.5.0"

[[Test]]
deps = ["InteractiveUtils", "Logging", "Random", "Serialization"]
uuid = "8dfed614-e22c-5e08-85e1-65c5234f0b40"

[[Unicode]]
uuid = "4ec0a83e-493e-50e2-b9ac-8f72acf5a8f5"
"""

# ╔═╡ Cell order:
# ╟─40e2df25-4309-4d8f-94d9-1f19f08decee
# ╟─e4796ed5-3edc-4e13-855f-ce98f9507b00
# ╠═72def700-f655-11eb-3347-0f460332501c
# ╟─43a2f15e-0f74-417f-9457-e285610a2a64
# ╠═c210e2e6-d985-4177-b516-82a158aaf505
# ╠═3da9ea6a-d89b-49a6-83cc-132a376a0f00
# ╠═83219521-6ee7-40b5-abd5-057ca98491fa
# ╟─3709c25a-d5f6-470d-9d76-f6019dda570c
# ╠═7ca8cc67-7da5-4a97-a2e2-ebb1314a999a
# ╠═344f30b1-efae-4286-8d95-665d65a0d6cb
# ╟─82368edb-f787-4e74-9c42-baca1198b800
# ╠═f602abe8-db66-46c0-b5d4-42faa2b872aa
# ╠═cd9c5c29-407c-4349-a1d5-45616936bfcf
# ╠═59a7d05b-e988-4ff2-93d5-8155e92bb2b9
# ╠═0f3ef489-767c-425d-870d-15c0a06ab3c0
# ╟─9deb7f76-be16-4030-92bc-b7c6cde58fc1
# ╟─2131a8a4-b0c0-49e5-ac7f-0e9977e39366
# ╟─7b5d9c9f-89aa-4e1e-9c47-7b77fa12b0ad
# ╟─f2a8a94b-4b39-48be-93c4-fa7661d03014
# ╟─1f993f0e-eb44-4bf6-91b7-971bb133974c
# ╟─5506f3db-89f0-40d6-a521-8f7818f866fc
# ╟─00000000-0000-0000-0000-000000000001
# ╟─00000000-0000-0000-0000-000000000002
