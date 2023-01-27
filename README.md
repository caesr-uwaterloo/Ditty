# Ditty: Directory-based Cache Coherence for Multicore Safety-critical Systems

- `aws`: useful scripts for linking the hls designs and RTL interconnect within the AWS server
- `intc_analysis`: the analysis for calculating interconnect latency
- `hls`: the implementation of the private caches and the LLC, the main designs are in `private_cache.cpp` and `llc.cpp`
- `hoplite-rt`: the RTL design of the HopliteRT interconnect
- `integration`: the integration of the design with the interconnect, where the caches/interconnects/LLCs are treated as blackboxes in SpinalHDL
