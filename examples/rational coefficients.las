<?xml version="1.0" encoding="utf-8"?>
<!--
	Copyright (C) 2014, 2018 Vaptistis Anogeianakis <nomad@cornercase.gr>

	This file is part of LostArt.

	LostArt is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	LostArt is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with LostArt.  If not, see <http://www.gnu.org/licenses/>.
-->

<!--
	Two squares of fixed size are anchored at constant
	distances relative to different corners of the window.

	Both quotients and fractional parts are used to
	represent rational numbers.
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
				<left>52.53802</left>
				<bottom>101.6</bottom>
				<right>118.8276</right>
				<top>52.21111</top>
			</sides>
			<borderSize>2</borderSize>
			<name>control0</name>
			<nameHeight>10</nameHeight>
		</control>
		<control>
			<sides>
				<left>181.2386</left>
				<bottom>220.4861</bottom>
				<right>241.8865</right>
				<top>163.3361</top>
			</sides>
			<borderSize>2</borderSize>
			<name>control1</name>
			<nameHeight>10</nameHeight>
		</control>
	</controls>
	<constraints>
		<constraint>
			<text>30.5mm</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>1</control>
				<side>left</side>
			</first-end-point>
			<second-end-point>
				<control>0</control>
				<side>left</side>
			</second-end-point>
			<first-local-side>84.08889</first-local-side>
			<second-local-side>91.08889</second-local-side>
		</constraint>
		<constraint>
			<text>20.93mm</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>0</control>
				<side>bottom</side>
			</first-end-point>
			<second-end-point>
				<control>1</control>
				<side>top</side>
			</second-end-point>
			<first-local-side>78.12787</first-local-side>
			<second-local-side>85.12787</second-local-side>
		</constraint>
		<constraint>
			<text>100.2px</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>1</control>
				<side>right</side>
			</first-end-point>
			<second-end-point>
				<control>1</control>
				<side>left</side>
			</second-end-point>
			<first-local-side>107.2722</first-local-side>
			<second-local-side>114.2722</second-local-side>
		</constraint>
		<constraint>
			<text>50.9px</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>1</control>
				<side>top</side>
			</first-end-point>
			<second-end-point>
				<control>1</control>
				<side>bottom</side>
			</second-end-point>
			<first-local-side>128.0214</first-local-side>
			<second-local-side>135.0213</second-local-side>
		</constraint>
		<constraint>
			<text>100mm</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>2</control>
				<side>right</side>
			</first-end-point>
			<second-end-point>
				<control>2</control>
				<side>left</side>
			</second-end-point>
			<first-local-side>142.9028</first-local-side>
			<second-local-side>149.9028</second-local-side>
		</constraint>
		<constraint>
			<text>50/3mm</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>2</control>
				<side>top</side>
			</first-end-point>
			<second-end-point>
				<control>2</control>
				<side>bottom</side>
			</second-end-point>
			<first-local-side>161.8714</first-local-side>
			<second-local-side>168.8714</second-local-side>
		</constraint>
		<constraint>
			<text>20/7px</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>2</control>
				<side>bottom</side>
			</first-end-point>
			<second-end-point>
				<control>0</control>
				<side>top</side>
			</second-end-point>
			<first-local-side>210.1781</first-local-side>
			<second-local-side>217.1781</second-local-side>
		</constraint>
		<constraint>
			<text>30/2px</text>
			<text-height>7</text-height>
			<first-end-point>
				<control>0</control>
				<side>right</side>
			</first-end-point>
			<second-end-point>
				<control>2</control>
				<side>right</side>
			</second-end-point>
			<first-local-side>185.6889</first-local-side>
			<second-local-side>192.6889</second-local-side>
		</constraint>
	</constraints>
</gui-model>
