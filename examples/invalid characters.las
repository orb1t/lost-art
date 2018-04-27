<?xml version="1.0" encoding="utf-8"?>
<!--
	Two controls are connected with constraints.

	The constraints contain characters that can't be part of
	valid linear constrains which will result in an compile-time
	error message.
-->
<gui-model>
	<controls>
		<control>
			<sides>
				<left>10</left>
				<bottom>10</bottom>
				<right>272.0833</right>
				<top>247.2222</top>
			</sides>
			<borderSize>2</borderSize>
			<name>Screen</name>
			<nameHeight>10</nameHeight>
		</control>
		<control>
			<sides>
				<left>29.61875</left>
				<bottom>200.025</bottom>
				<right>86.38805</right>
				<top>153.1056</top>
			</sides>
			<borderSize>2</borderSize>
			<name>Alice</name>
			<nameHeight>10</nameHeight>
		</control>
		<control>
			<sides>
				<left>155.851</left>
				<bottom>205.6694</bottom>
				<right>225.3141</right>
				<top>117.1222</top>
			</sides>
			<borderSize>2</borderSize>
			<name>Bob</name>
			<nameHeight>10</nameHeight>
		</control>
	</controls>
	<constraints>
		<constraint>
			<text>!!</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>2</control>
				<side>left</side>
			</first-end-point>
			<second-end-point>
				<control>1</control>
				<side>right</side>
			</second-end-point>
			<first-local-side>190.813599</first-local-side>
			<second-local-side>197.813599</second-local-side>
		</constraint>
		<constraint>
			<text>@</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>2</control>
				<side>top</side>
			</first-end-point>
			<second-end-point>
				<control>1</control>
				<side>top</side>
			</second-end-point>
			<first-local-side>120.792969</first-local-side>
			<second-local-side>127.792969</second-local-side>
		</constraint>
	</constraints>
</gui-model>